/**
 * @file arb_page.c
 * @brief arbitrary waveform / modulation page
 * @author Duboisset Philippe
 * @version 0.1b
 * @date (yyyy-mm-dd) 2014-04-05
 *
 * Copyright (C) <2014>  Duboisset Philippe <duboisset.philippe@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gui_common.h"
#include "arb_page.h"
#include "arb_process.h"
#include "mod.h"
#include "mod_page.h"
#include "ana.h"
#include "dac.h"
#include "dds.h"

#define GRAPH_WIDTH     ARB_WAVEFORM_DEPTH
#define GRAPH_HEIGHT    121


/*widgets signals*/
enum {

  /*value boxes*/
  SIG_RVAL_FREQ = 1,
  SIG_RVAL_VPP,
  SIG_RVAL_OFFSET,
  SIG_RVAL_VMAX,
  SIG_RVAL_VMIN,

  /*graph*/
  SIG_GRAPH,

  /*buttons*/
  SIG_BTN_RUN,
  SIG_BTN_PAUSE,
  SIG_BTN_BROWSE,
  SIG_BTN_MOD,
  SIG_BTN_HOME = 0xFFFF
};


/**
 * local variables
 */
static voltage_ctrl_st voltages;
static arb_st arb;
static g_obj_st *pFreqVal, *pVppVal, *pOffsetVal, *pObjGraph;
static int8_t var8; /*used to store increment coming from the rotary button*/


/**
 * local functions
 */
static void ARB_PageHandler(signal_t sig);
static void ARB_DrawGraphArea(const rect_st *rec);
static void ARB_PageSelectWaveform(signal_t sig);
static void ARB_PageSelectWaveformHandler(signal_t sig);
static void LocalInit(void);
static void LocalExit(void);
static void LockValueBox(void);


/**
 * @function ARB_Page
 * @brief arbitrary waveform page
 * @param signal_t sig: unused
 * @return none
 */
void ARB_Page(signal_t sig) {

  rect_st rec;

  LocalInit();

  /*background & (PWM out / DDS out / ANA in)*/
  GUI_ClearAll();
  DrawBackground();
  DrawIO(false, true, arb.waveformType == ARB_WAVE_ANA_IN? true: false);

  /*frequency value box*/
  SetFont(G_FONT_DEFAULT);
  rec = GUI_Rect(8, 160, 113, 32);
  pFreqVal = GUI_W_RotaryValueAdd(&rec, &arb.frequency, &var8, "Hz", 0);
  GUI_SetSignal(E_RELEASED_TO_PUSHED, SIG_RVAL_FREQ);
  GUI_W_RotaryValueSetDotPos(NULL, 1);
  GUI_W_RotaryValueSetMinMax(NULL, 1, 20000);
  if(arb.waveformType == ARB_WAVE_NOISE || arb.waveformType == ARB_WAVE_WAV
       || arb.waveformType == ARB_WAVE_ANA_IN) GUI_ObjSetDisabled(pFreqVal, true);

  /*Vpp value box*/
  rec = GUI_Rect(8, 195, 113, 32);
  pVppVal = GUI_W_RotaryValueAdd(&rec, &voltages.vpp, &var8, "V", G_IMG_RVAL_VPP);
  GUI_SetSignal(E_RELEASED_TO_PUSHED, SIG_RVAL_VPP);
  GUI_W_RotaryValueSetDotPos(NULL, 2);
  GUI_W_RotaryValueSetMinMax(NULL, ANA_OUTPUT_MIN_VPP, ANA_OUTPUT_MAX * 2);
  if(modType == MOD_AM) GUI_ObjSetDisabled(NULL, true);

  /*offset value box*/
  rec = GUI_Rect(8, 230, 113, 32);
  pOffsetVal = GUI_W_RotaryValueAdd(&rec, &voltages.offset, &var8, "V", G_IMG_RVAL_VOFFSET);
  GUI_SetSignal(E_RELEASED_TO_PUSHED, SIG_RVAL_OFFSET);
  GUI_W_RotaryValueSetDotPos(NULL, 2);
  GUI_W_RotaryValueSetMinMax(NULL, -ANA_OUTPUT_MAX, ANA_OUTPUT_MAX);
  if(modType == MOD_AM) GUI_ObjSetDisabled(NULL, true);

  /*no value box is selected when entering the page*/
  LockValueBox();

  /*PLAY button*/
  rec = GUI_Rect(8, 270, 41, 41);
  GUI_W_RadioImgAdd(&rec, G_DDS_PLAY0, &arb.run, 1);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_BTN_RUN);

  /*PAUSE button*/
  rec.x += rec.w + 4;
  GUI_W_RadioImgAdd(&rec, G_DDS_PAUSE0, &arb.run, 0);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_BTN_PAUSE);

  /*...*/
  rec.x += rec.w + 4;
  GUI_W_RadioImgAdd(&rec, G_DDS_BROWSE0, NULL, 0);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_BTN_BROWSE);

  /*modulation*/
  rec.x += rec.w + 4;
  GUI_W_RadioImgAdd(&rec, G_DDS_MOD0 + (modType == MOD_OFF? 0 : 2), &arb.run, DDS_WAVE_DC);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_BTN_MOD);

  /*home button*/
  rec.x += rec.w + 4;
  GUI_W_RadioImgAdd(&rec, G_DDS_BACK0, NULL, 0);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_BTN_HOME);

  /*main rotary button*/
  rec = GUI_Rect(130, 160, 102, 102);
  GUI_W_RotaryButtonAdd(&rec, &var8, ROTARY_BTN_GR_30_DEG);

  /*graph*/
  rec = GUI_Rect(8, 23, 224, 134);
  GUI_W_ImgAdd(&rec, 0, DISPLAY_TRANSPARENT);
  ARB_DrawGraphArea(&rec);

  /*jump to the handler*/
  GUI_SetUserTask(ARB_PageHandler);
}


/**
 * @function ARB_DrawGraphArea
 * @brief draw the graph area, according to the waveform
 * @param const rect_st *dst: external rec dimension / position
 * @return none
 */
static void ARB_DrawGraphArea(const rect_st *dst) {

  rect_st rec;
  uint8_t str[2] = {0, 0};

  switch(arb.waveformType) {

    /*noise & .wav: just display a symbol and some text*/
    case ARB_WAVE_NOISE:
    case ARB_WAVE_WAV:

      GUI_SetAlign(G_ALIGN_H_CENTER | G_ALIGN_V_CENTER);

      /*display waveform symbol*/
      SetFont(G_FONT_WAVE_SYMBOL);
      str[0] = arb.waveformType + 1;
      GUI_W_TextAdd(dst, str);
      SetFont(G_FONT_DEFAULT);

      /*waveform text description*/
      rec = *dst;
      rec.y += rec.h / 2; rec.h /= 2;
      GUI_W_TextAdd(&rec, (arb.waveformType == ARB_WAVE_NOISE)? "random noise": ".wav file");

      GUI_SetAlign(0);
      break;

    /*other waveform: display graph*/
    default:
      rec.w = GRAPH_WIDTH;
      rec.h = GRAPH_HEIGHT;
      rec.x = dst->x + ((coord_t)dst->w - rec.w) / 2;
      rec.y = dst->y + ((coord_t)dst->h - rec.h) / 2;
      GUI_W_GraphSetGridSpacing(20, 20);
      pObjGraph = GUI_W_GraphAdd(&rec, GRAPH_GRID_DOT_HV, 0);
      GUI_W_GraphAddCurveToGraph(NULL, arb.waveform, P2D_Color(255, 192, 0));
      GUI_SetSignal(E_PUSHED, SIG_GRAPH);
      break;
  }
}


/**
 * @function ARB_PageHandler
 * @brief ARB page handler
 * @param signal_t sig: signal coming from widgets
 * @return none
 */
static void ARB_PageHandler(signal_t sig) {

  coord_t x, y;
  static coord_t oldX = -1, oldY = -1;
  g_obj_st *selectedObj = NULL;
  bool bRefresh;
  static timer_t tmRefreshAnaIn = 0;

  /*in waveform == ANA_IN, draw cyclically some samples on the graph*/
  if(arb.waveformType == ARB_WAVE_ANA_IN && IsTimerElapsed(tmRefreshAnaIn)) {
    tmRefreshAnaIn = GetPeriodicTimeout(50);
    GUI_W_GraphAddSampleToCurve(pObjGraph, 0, ANA_GetInput() >> 8);
  }

  /*handle user event*/
  switch(sig) {

    /*no signal*/
    case 0:
      DAC_Update(&voltages, false);
      ARB_UpdateFrequency(&arb, false);
      if(GUI_ObjIsPressed(pObjGraph) == false) {
        oldX = oldY = -1;
      }
      break;

    /*PLAY button*/
    case SIG_BTN_RUN:
      ARB_Run(&arb);
      break;

    /*PAUSE button*/
    case SIG_BTN_PAUSE:
      ARB_Pause(&arb);
      break;

    /*browse*/
    case SIG_BTN_BROWSE:
      ARB_Stop(&arb);
      GUI_SetUserTask(ARB_PageSelectWaveform);
      break;

    /*modulation*/
    case SIG_BTN_MOD:
      ARB_Stop(&arb);
      GUI_SetUserTask(MOD_Page);
      break;

    /*home*/
    case SIG_BTN_HOME:
      LocalExit();
      GUI_SetUserTask(GUI_MainMenu);
      break;

    /*graph update*/
    case SIG_GRAPH:
      GUI_W_GraphGetTouch(pObjGraph, &x, &y);
      bRefresh = ARB_UpdateWaveform(&arb, GRAPH_HEIGHT, oldX, oldY, x, y);
      oldX = x;
      oldY = y;
      if(bRefresh) GUI_ObjSetNeedRefresh(pObjGraph, true);
      break;

    /*at this point, signal comes from a valuebox*/
    default:
      /*lock all value box, except the selected one*/
      LockValueBox();
      if(sig == SIG_RVAL_FREQ)        selectedObj = pFreqVal;
      else if(sig == SIG_RVAL_VPP)    selectedObj = pVppVal;
      else                            selectedObj = pOffsetVal;
      GUI_W_RotaryValueLock(selectedObj, false);
      break;
  }
}


/**
 * @function ARB_PageSelectWaveform
 * @brief waveform selection page
 * @param signal_t sig: unused
 * @return none
 */
static void ARB_PageSelectWaveform(signal_t sig) {

  rect_st rec;
  coord_t spacing = 5, startX = 10, startY = 10, width = 50;
  uint16_t ii, btnPerLine = 4;
  uint8_t str[2] = {0,0};

  /*background*/
  GUI_ClearAll();
  DrawBackground();

  /*display all possible waveforms*/
  SetFont(G_FONT_WAVE_SYMBOL);
  rec.w = rec.h = width;
  for(ii = 0; ii < _ARB_WAV_COUNT; ii++) {
    str[0] = ii + 1;
    rec.x = startX + (ii % btnPerLine) * (rec.w + spacing);
    rec.y = startY + (ii / btnPerLine) * (rec.h + spacing);
    GUI_W_ButtonAdd(&rec, str, 0);
    GUI_SetSignal(E_PUSHED_TO_RELEASED, str[0]);
  }

  SetFont(G_FONT_DEFAULT);

  /*home button*/
  rec.x = 188; rec.y = 270;
  GUI_W_RadioImgAdd(&rec, G_DDS_BACK0, NULL, 0);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_BTN_HOME);

  /*jump to the handler*/
  GUI_SetUserTask(ARB_PageSelectWaveformHandler);
}


/**
 * @function ARB_PageSelectWaveformHandler
 * @brief ARB_PageSelectWaveform handler
 * @param signal_t sig: signal coming from widgets
 * @return none
 */
static void ARB_PageSelectWaveformHandler(signal_t sig) {
  if(sig != 0) {
    if(sig != SIG_BTN_HOME) {
      ARB_SetWaveform(&arb, sig - 1);
    }
    GUI_SetUserTask(ARB_Page);
  }
}


/**
 * @function LocalInit
 * @brief initialize the ARB / MOD handler
 * @param none
 * @return none
 */
static void LocalInit(void) {

  static bool bFirstRun = true;

  ARB_Init(&arb, bFirstRun);
  DAC_Init(&voltages, 0, bFirstRun);

  /*force vo / vpp in case of AM*/
  if(modType == MOD_AM) {
    voltages.offset = 0;
    voltages.vpp = modAM.vppMax;
  }
  bFirstRun = false;
}


/**
 * @function LocalExit
 * @brief
 * @param none
 * @return none
 */
static void LocalExit(void) {
  ARB_Stop(&arb);
}


/**
 * @function LockValueBox
 * @brief lock all valueboxes
 * @param none
 * @return none
 */
static void LockValueBox(void) {
  GUI_W_RotaryValueLock(pFreqVal, true);
  GUI_W_RotaryValueLock(pVppVal, true);
  GUI_W_RotaryValueLock(pOffsetVal, true);
}
