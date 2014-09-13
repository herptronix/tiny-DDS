/**
 * @file dds_page.c
 * @brief basic dds page
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
#include "dds_page.h"
#include "ana.h"
#include "dac.h"
#include "dds.h"


/*widgets signals*/
enum {

  /*value boxes*/
  SIG_RVAL_FREQ = 1,
  SIG_RVAL_VPP,
  SIG_RVAL_OFFSET,
  SIG_RVAL_VMAX,
  SIG_RVAL_VMIN,

  /*buttons*/
  SIG_BTN_RUN,
  SIG_BTN_PAUSE,
  _SIG_WAVE_BTN,
  SIG_BTN_SINUS = _SIG_WAVE_BTN + DDS_WAVE_SINUS,
  SIG_BTN_TRIG = _SIG_WAVE_BTN + DDS_WAVE_TRIG,
  SIG_BTN_DC = _SIG_WAVE_BTN + DDS_WAVE_DC,
  SIG_BTN_HOME
};


/**
 * local variables
 */
static voltage_ctrl_st voltages;
static dds_ctrl_st dds;
static g_obj_st *pFreqVal, *pVppVal, *pOffsetVal, *pVminVal, *pVmaxVal;
static g_obj_st *oldSelectedObj;
static uint8_t oldSelectedDigit;
static int8_t var8; /*used to store increment coming from the rotary button*/


/**
 * local functions
 */
static void DDS_PageHandler(signal_t sig);
static void LocalInit(void);
static void LocalExit(void);
static void SetWave(uint8_t wave);
static void LockValueBox(void);


/**
 * @function DDS_Page
 * @brief DDS handling (simple page, with sinus & triangle waveform)
 * @param signal_t sig: unused
 * @return none
 */
void DDS_Page(signal_t sig) {

  rect_st rec;

  /*background & (PWM out / DDS out / ANA in)*/
  GUI_ClearAll();
  DrawBackground();
  DrawIO(false, true, false);

  /*frequency value box*/
  SetFont(G_FONT_BIG);
  rec = GUI_Rect(8, 23, 224, 48);
  pFreqVal = GUI_W_RotaryValueAdd(&rec, &dds.frequency, &var8, "Hz", 0);
  GUI_SetSignal(E_RELEASED_TO_PUSHED, SIG_RVAL_FREQ);
  GUI_W_RotaryValueSetDotPos(NULL, 1);

  /*Vpp value box*/
  SetFont(G_FONT_DEFAULT);
  rec = GUI_Rect(8, 75, 110, 32);
  pVppVal = GUI_W_RotaryValueAdd(&rec, &voltages.vpp, &var8, "V", G_IMG_RVAL_VPP);
  GUI_SetSignal(E_RELEASED_TO_PUSHED, SIG_RVAL_VPP);
  GUI_W_RotaryValueSetDotPos(NULL, 2);

  /*offset value box*/
  rec = GUI_Rect(121, 75, 110, 32);
  pOffsetVal = GUI_W_RotaryValueAdd(&rec, &voltages.offset, &var8, "V", G_IMG_RVAL_VOFFSET);
  GUI_SetSignal(E_RELEASED_TO_PUSHED, SIG_RVAL_OFFSET);
  GUI_W_RotaryValueSetDotPos(NULL, 2);

  /*Vmin value box*/
  rec = GUI_Rect(8, 111, 110, 32);
  pVminVal = GUI_W_RotaryValueAdd(&rec, &voltages.vmin, &var8, "V", G_IMG_RVAL_VMIN);
  GUI_SetSignal(E_RELEASED_TO_PUSHED, SIG_RVAL_VMIN);
  GUI_W_RotaryValueSetDotPos(NULL, 2);

  /*Vmax value box*/
  rec = GUI_Rect(121, 111, 110, 32);
  pVmaxVal = GUI_W_RotaryValueAdd(&rec, &voltages.vmax, &var8, "V", G_IMG_RVAL_VMAX);
  GUI_SetSignal(E_RELEASED_TO_PUSHED, SIG_RVAL_VMAX);
  GUI_W_RotaryValueSetDotPos(NULL, 2);

  /*no value box is selected when entering the page*/
  LockValueBox();

  /*PLAY button*/
  rec = GUI_Rect(8, 150, 41, 41);
  GUI_W_RadioImgAdd(&rec, G_DDS_PLAY0, &dds.run, 1);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_BTN_RUN);

  /*PAUSE button*/
  rec.x += rec.w + 3;
  GUI_W_RadioImgAdd(&rec, G_DDS_PAUSE0, &dds.run, 0);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_BTN_PAUSE);

  /*SINUS button*/
  rec.x += rec.w + 7;
  GUI_W_RadioImgAdd(&rec, G_DDS_SINE0, &dds.wave, DDS_WAVE_SINUS);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_BTN_SINUS);

  /*TRIANGLE button*/
  rec.x += rec.w + 3;
  GUI_W_RadioImgAdd(&rec, G_DDS_TRIG0, &dds.wave, DDS_WAVE_TRIG);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_BTN_TRIG);

  /*DC button*/
  rec.x += rec.w + 3;
  GUI_W_RadioImgAdd(&rec, G_DDS_DC0, &dds.wave, DDS_WAVE_DC);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_BTN_DC);

  /*home button*/
  rec.y = 265;
  GUI_W_RadioImgAdd(&rec, G_DDS_BACK0, NULL, 0);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_BTN_HOME);

  /*main rotary button*/
  rec = GUI_Rect(69, 203, 102, 102);
  GUI_W_RotaryButtonAdd(&rec, &var8, ROTARY_BTN_GR_30_DEG);

  /*local init & jump to the handler*/
  LocalInit();
  GUI_SetUserTask(DDS_PageHandler);
}


/**
 * @function DDS_PageHandler
 * @brief DDS page handler
 * @param signal_t sig: signal coming from widgets
 * @return none
 */
static void DDS_PageHandler(signal_t sig) {

  bool bClear = false;
  g_obj_st *selectedObj = NULL;

  /*handle user event*/
  switch(sig) {

    /*no signal? update registers*/
    case 0:
      DDS_FequencyUpdate(&dds, false);
      DAC_Update(&voltages, false);
      break;

    /*home*/
    case SIG_BTN_HOME:
      LocalExit();
      GUI_SetUserTask(GUI_MainMenu);
      break;

    /*sinus / triangle / DC button*/
    case SIG_BTN_SINUS:
    case SIG_BTN_TRIG:
    case SIG_BTN_DC:
      SetWave(sig - _SIG_WAVE_BTN);
      bClear = true;
      break;

    /*PLAY button*/
    case SIG_BTN_RUN:
      DDS_Run(&dds);
      bClear = true;
      break;

    /*PAUSE button*/
    case SIG_BTN_PAUSE:
      DDS_Pause(&dds);
      bClear = true;
      break;

    /*at this point, signal comes from a valuebox*/
    default:

      /*lock all value box, except the selected one*/
      LockValueBox();
      if(sig == SIG_RVAL_FREQ)        selectedObj = pFreqVal;
      else if(sig == SIG_RVAL_VPP)    selectedObj = pVppVal;
      else if(sig == SIG_RVAL_OFFSET) selectedObj = pOffsetVal;
      else if(sig == SIG_RVAL_VMIN)   selectedObj = pVminVal;
      else                            selectedObj = pVmaxVal;
      GUI_W_RotaryValueLock(selectedObj, false);
      break;
  }

  /*action on a widget != of a value box clear*/
  if(bClear) {
    oldSelectedObj = NULL;
    oldSelectedDigit = 255;
  }
}


/**
 * @function LocalInit
 * @brief initialize the DDS page
 * @param none
 * @return none
 */
static void LocalInit(void) {

  static bool bFirstRun = true;
  DAC_Init(&voltages, 0, bFirstRun);
  DDS_Init(&dds, 0, bFirstRun);
  SetWave(dds.wave);

  bFirstRun = false;
}


/**
 * @function LocalExit
 * @brief clean exit
 * @param none
 * @return none
 */
static void LocalExit(void) {
  DDS_Stop(&dds);
}


/**
 * @function SetWave
 * @brief set the selected wave & apply analog limitations on valueboxes
 * @param uint8_t wave: see dds.h
 * @return none
 */
static void SetWave(uint8_t wave) {

  DDS_SetWave(&dds, wave);

  /*configure min/max of each valuebox according to DDS_SetWave() result*/
  GUI_W_RotaryValueSetMinMax(pVppVal, dds.lim.vppMin, dds.lim.vppMax);
  GUI_W_RotaryValueSetMinMax(pOffsetVal, dds.lim.offsetMin, dds.lim.offsetMax);
  GUI_W_RotaryValueSetMinMax(pFreqVal, dds.lim.freqMin, dds.lim.freqMax);
  GUI_W_RotaryValueSetMinMax(pVminVal, -ANA_OUTPUT_MAX, ANA_OUTPUT_MAX);
  GUI_W_RotaryValueSetMinMax(pVmaxVal, -ANA_OUTPUT_MAX, ANA_OUTPUT_MAX);

  /*disable value boxes (except offset) if DC waveform*/
  if(wave == DDS_WAVE_DC) {
    GUI_ObjSetDisabled(pFreqVal, true);
    GUI_ObjSetDisabled(pVppVal, true);
    GUI_ObjSetDisabled(pVminVal, true);
    GUI_ObjSetDisabled(pVmaxVal, true);
    LockValueBox();
    GUI_W_RotaryValueLock(pOffsetVal, false);
  }
  else {
    GUI_ObjSetDisabled(pFreqVal, false);
    GUI_ObjSetDisabled(pVppVal, false);
    GUI_ObjSetDisabled(pVminVal, false);
    GUI_ObjSetDisabled(pVmaxVal, false);
  }
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
  GUI_W_RotaryValueLock(pVminVal, true);
  GUI_W_RotaryValueLock(pVmaxVal, true);
}
