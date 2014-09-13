/**
 * @file mod_page.c
 * @brief modulation config. page
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

#include "ana.h"
#include "mod.h"
#include "gui_common.h"
#include "mod_page.h"
#include "arb_page.h"


/**
 * local variables
 */
static uint8_t mod;
static int8_t var8;
static g_obj_st *pAmFreqVal, *pAmVminVal, *pAmVmaxVal, *pFmFreqMin, *pFmFreqMax, *pRotBtn;


/**
 * local functions
 */
static void MOD_PageHandler(signal_t sig);
static void LocalInit(void);
static void RefreshSelectedGroup(void);
static void LockValueBox(void);


enum {

  SIG_AM_RVAL_FREQ = 1,
  SIG_AM_RVAL_VMIN,
  SIG_AM_RVAL_VMAX,
  SIG_FM_RVAL_FREQ_MIN,
  SIG_FM_RVAL_FREQ_MAX,
  SIG_BTN_HOME,

  _SIG_MOD = 100,
  SIG_MOD_OFF = _SIG_MOD,
  SIG_MOD_AM,
  SIG_MOD_FM
};


/**
 * @function MOD_Page
 * @brief modulation config. page
 * @param signal_t sig: unused
 * @return none
 */
void MOD_Page(signal_t sig) {

  rect_st rec;
  LocalInit();

  /*background*/
  GUI_ClearAll();
  DrawBackground();

  /**
   * common widgets
   */
  /*main rotary button*/
  rec = GUI_Rect(69, 203, 102, 102);
  pRotBtn = GUI_W_RotaryButtonAdd(&rec, &var8, ROTARY_BTN_GR_30_DEG);

  /*home button*/
  rec.x = 188; rec.y = 270;
  GUI_W_RadioImgAdd(&rec, G_DDS_BACK0, NULL, 0);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_BTN_HOME);

  /*modulation radios*/
  mod = modType;
  SetFont(G_FONT_DEFAULT);

  rec = GUI_Rect(10,10,70,29);
  GUI_W_RadioAdd(&rec, "OFF", &mod, MOD_OFF);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_MOD_OFF);

  rec.x += rec.w + 5;
  GUI_W_RadioAdd(&rec, "AM", &mod, MOD_AM);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_MOD_AM);

  rec.x += rec.w + 5;
  GUI_W_RadioAdd(&rec, "FM", &mod, MOD_FM);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_MOD_FM);

  /**
   * AM modulation widgets
   */
  /*frequency value box*/
  SetFont(G_FONT_DEFAULT);
  rec = GUI_Rect(8, 48, 224, 32);
  pAmFreqVal = GUI_W_RotaryValueAdd(&rec, &modAM.frequency, &var8, "Hz", G_IMG_RVAL_FMIN);
  GUI_SetSignal(E_RELEASED_TO_PUSHED, SIG_AM_RVAL_FREQ);
  GUI_W_RotaryValueSetDotPos(NULL, 1);
  GUI_W_RotaryValueSetMinMax(NULL, ANA_FREQ_MIN, ANA_FREQ_SINE_MAX);

  /*Vmin value box*/
  rec = GUI_Rect(8, 83, 110, 32);
  pAmVminVal = GUI_W_RotaryValueAdd(&rec, &modAM.vppMin, &var8, "V", G_IMG_RVAL_VPPMIN);
  GUI_SetSignal(E_RELEASED_TO_PUSHED, SIG_AM_RVAL_VMIN);
  GUI_W_RotaryValueSetDotPos(NULL, 2);
  GUI_W_RotaryValueSetMinMax(NULL, ANA_OUTPUT_MIN_VPP, ANA_OUTPUT_MAX * 2);

  /*Vmax value box*/
  rec = GUI_Rect(121, 83, 110, 32);
  pAmVmaxVal = GUI_W_RotaryValueAdd(&rec, &modAM.vppMax, &var8, "V", G_IMG_RVAL_VPP);
  GUI_SetSignal(E_RELEASED_TO_PUSHED, SIG_AM_RVAL_VMAX);
  GUI_W_RotaryValueSetDotPos(NULL, 2);
  GUI_W_RotaryValueSetMinMax(NULL, ANA_OUTPUT_MIN_VPP, ANA_OUTPUT_MAX * 2);

  /**
   * FM modulation widgets
   */
  rec = GUI_Rect(8, 127, 224, 32);
  pFmFreqMin = GUI_W_RotaryValueAdd(&rec, &modFM.freqMin, &var8, "Hz", G_IMG_RVAL_FMIN);
  GUI_SetSignal(E_RELEASED_TO_PUSHED, SIG_FM_RVAL_FREQ_MIN);
  GUI_W_RotaryValueSetMinMax(NULL, ANA_FREQ_MIN, ANA_FREQ_SINE_MAX);
  GUI_W_RotaryValueSetDotPos(NULL, 1);
  GUI_W_RotaryValueSetMinMax(NULL, ANA_FREQ_MIN, ANA_FREQ_SINE_MAX);

  rec = GUI_Rect(8, 161, 224, 32);
  pFmFreqMax = GUI_W_RotaryValueAdd(&rec, &modFM.freqMax, &var8, "Hz", G_IMG_RVAL_FMAX);
  GUI_SetSignal(E_RELEASED_TO_PUSHED, SIG_FM_RVAL_FREQ_MAX);
  GUI_W_RotaryValueSetMinMax(NULL, ANA_FREQ_MIN, ANA_FREQ_SINE_MAX);
  GUI_W_RotaryValueSetDotPos(NULL, 1);
  GUI_W_RotaryValueSetMinMax(NULL, ANA_FREQ_MIN, ANA_FREQ_SINE_MAX);

  /*jump to the right hnd*/
  RefreshSelectedGroup();
  GUI_SetUserTask(MOD_PageHandler);
}


/**
 * @function MOD_PageHandler
 * @brief MOD_Page handler
 * @param signal_t sig: signal coming from widgets
 * @return none
 */
static void MOD_PageHandler(signal_t sig) {

  switch(sig) {

    /*home*/
    case SIG_BTN_HOME:
      GUI_SetUserTask(ARB_Page);
      break;

    /*modulation radios*/
    case SIG_MOD_OFF:
    case SIG_MOD_AM:
    case SIG_MOD_FM:
      modType = sig - _SIG_MOD;
      mod = modType;
      RefreshSelectedGroup();
      break;

    /*mod AM, frequency value box*/
    case SIG_AM_RVAL_FREQ:
      LockValueBox();
      GUI_W_RotaryValueLock(pAmFreqVal, false);
      break;

    /*mod AM, vpp min value box*/
    case SIG_AM_RVAL_VMIN:
      LockValueBox();
      GUI_W_RotaryValueLock(pAmVminVal, false);
      break;

    /*mod AM, vpp max value box*/
    case SIG_AM_RVAL_VMAX:
      LockValueBox();
      GUI_W_RotaryValueLock(pAmVmaxVal, false);
      break;

    /*mod FM, min frequency value box*/
    case SIG_FM_RVAL_FREQ_MIN:
      LockValueBox();
      GUI_W_RotaryValueLock(pFmFreqMin, false);
      break;

    /*mod FM, max frequency value box*/
    case SIG_FM_RVAL_FREQ_MAX:
      LockValueBox();
      GUI_W_RotaryValueLock(pFmFreqMax, false);
      break;

    default:
      break;
  }
}


/**
 * @function LocalInit
 * @brief initialize the AM / FM
 * @param none
 * @return none
 */
static void LocalInit(void) {

  static bool bFirstRun = true;

  if(bFirstRun) {

    /*default AM: 1MHz, max voltage amplitude*/
    modAM.frequency = 10000000;
    modAM.vppMin = ANA_OUTPUT_MIN_VPP;
    modAM.vppMax = ANA_OUTPUT_MAX * 2;

    /*default FM: fmin = 900kHz, fmax = 1.1MHz*/
    modFM.freqMin = 9000000;
    modFM.freqMax = 11000000;
    bFirstRun = false;
  }
}


/**
 * @function RefreshSelectedGroup
 * @brief enable / disable widgets according to the current modulation type
 * @param none
 * @return none
 */
static void RefreshSelectedGroup(void) {
  if(modType == MOD_OFF) {
    GUI_ObjSetDisabled(pAmFreqVal, true);
    GUI_ObjSetDisabled(pAmVminVal, true);
    GUI_ObjSetDisabled(pAmVmaxVal, true);
    GUI_ObjSetDisabled(pFmFreqMin, true);
    GUI_ObjSetDisabled(pFmFreqMax, true);
    GUI_ObjSetDisabled(pRotBtn, true);
  }
  else if(modType == MOD_AM) {
    GUI_ObjSetDisabled(pAmFreqVal, false);
    GUI_ObjSetDisabled(pAmVminVal, false);
    GUI_ObjSetDisabled(pAmVmaxVal, false);
    GUI_ObjSetDisabled(pFmFreqMin, true);
    GUI_ObjSetDisabled(pFmFreqMax, true);
    GUI_ObjSetDisabled(pRotBtn, false);
  }
  else {
    GUI_ObjSetDisabled(pAmFreqVal, true);
    GUI_ObjSetDisabled(pAmVminVal, true);
    GUI_ObjSetDisabled(pAmVmaxVal, true);
    GUI_ObjSetDisabled(pFmFreqMin, false);
    GUI_ObjSetDisabled(pFmFreqMax, false);
    GUI_ObjSetDisabled(pRotBtn, false);
  }

  /*page is under creation: unselect all value boxes*/
  LockValueBox();
}


/**
 * @function LockValueBox
 * @brief lock all valueboxes
 * @param none
 * @return none
 */
static void LockValueBox(void) {
  GUI_W_RotaryValueLock(pAmFreqVal, true);
  GUI_W_RotaryValueLock(pAmVminVal, true);
  GUI_W_RotaryValueLock(pAmVmaxVal, true);
  GUI_W_RotaryValueLock(pFmFreqMin, true);
  GUI_W_RotaryValueLock(pFmFreqMax, true);
}
