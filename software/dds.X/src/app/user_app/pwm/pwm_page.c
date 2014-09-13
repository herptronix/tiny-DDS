/**
 * @file pwm_page.c
 * @brief PWM page
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
#include "pwm_page.h"
#include "pwm.h"


/*widgets signals*/
enum {

  /*value boxes*/
  SIG_RVAL_FREQ = 1,
  SIG_RVAL_TH,
  SIG_RVAL_TL,
  SIG_RVAL_DUTY,
  SIG_LINK_TL_TH,

  /*buttons*/
  SIG_BTN_RUN,
  SIG_BTN_PAUSE,
  SIG_BTN_HOME
};


/**
 * local variables
 */
static pwm_ctrl_st pwm;
static g_obj_st *pFreqVal, *pThVal, *pTlVal, *pDutyVal, *pBtnLink;
static int8_t var8; /*used to store increment coming from the rotary button*/


/**
 * local functions
 */
static void PWM_PageHandler(signal_t sig);
static void LocalInit(void);
static void LocalExit(void);
static void LockValueBox(void);


/**
 * @function PWM_Page
 * @brief PWM page
 * @param signal_t sig: unused
 * @return none
 */
void PWM_Page(signal_t sig) {

  rect_st rec;

  LocalInit();

  /*background & (PWM out / DDS out / ANA in)*/
  GUI_ClearAll();
  DrawBackground();
  DrawIO(true, false, false);

  /*frequency value box*/
  SetFont(G_FONT_BIG);
  rec = GUI_Rect(8, 23, 224, 48);
  pFreqVal = GUI_W_RotaryValueAdd(&rec, &pwm.freq, &var8, "Hz", 0);
  GUI_SetSignal(E_RELEASED_TO_PUSHED, SIG_RVAL_FREQ);
  GUI_W_RotaryValueSetMinMax(NULL, pwm.freqMin, pwm.freqMax);
  GUI_W_RotaryValueSetDotPos(NULL, 1);

  /*Th value box*/
  SetFont(G_FONT_DEFAULT);
  rec = GUI_Rect(42, 75, 190, 32);
  pThVal = GUI_W_RotaryValueAdd(&rec, &pwm.th, &var8, "ns", G_IMG_RVAL_TH);
  GUI_SetSignal(E_RELEASED_TO_PUSHED, SIG_RVAL_TH);
  GUI_W_RotaryValueSetMinMax(NULL, pwm.thMin, pwm.thMax);
  GUI_W_RotaryValueSetDotPos(NULL, 0);

  /*Tl value box*/
  rec = GUI_Rect(42, 111, 190, 32);
  pTlVal = GUI_W_RotaryValueAdd(&rec, &pwm.tl, &var8, "ns", G_IMG_RVAL_TL);
  GUI_SetSignal(E_RELEASED_TO_PUSHED, SIG_RVAL_TL);
  GUI_W_RotaryValueSetMinMax(NULL, pwm.tlMin, pwm.tlMax);
  GUI_W_RotaryValueSetDotPos(NULL, 0);

  /*chain TL/TH button*/
  rec = GUI_Rect(8, 75, 31, 68);
  pBtnLink = GUI_W_ButtonAdd(&rec, NULL, G_IMG_RVAL_CHAIN);
  GUI_W_ButtonShade(pBtnLink, !(pwm.linkTlTh));
  GUI_SetSignal(E_RELEASED_TO_PUSHED, SIG_LINK_TL_TH);

  /*duty value box*/
  rec = GUI_Rect(8, 147, 110, 32);
  pDutyVal = GUI_W_RotaryValueAdd(&rec, &pwm.duty, &var8, "%", G_IMG_RVAL_ALPHA);
  GUI_SetSignal(E_RELEASED_TO_PUSHED, SIG_RVAL_DUTY);
  GUI_W_RotaryValueSetDotPos(NULL, 1);
  GUI_W_RotaryValueSetMinMax(NULL, pwm.dutyMin, pwm.dutyMax);

  /*resolution value box*/
  rec = GUI_Rect(121, 147, 110, 32);
  GUI_W_RotaryValueAdd(&rec, &pwm.resolution, &var8, "bits", 0);
  GUI_W_RotaryValueSetDotPos(NULL, 1);
  GUI_W_RotaryValueSetMinMax(NULL, 0, 999);
  GUI_W_RotaryValueLock(NULL, true);
  GUI_ObjSetDisabled(NULL, true);

  /*no value box is selected when entering the page*/
  LockValueBox();

  /*PLAY button*/
  rec = GUI_Rect(8, 210, 41, 41);
  GUI_W_RadioImgAdd(&rec, G_DDS_PLAY0, &pwm.run, 1);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_BTN_RUN);

  /*PAUSE button*/
  rec = GUI_Rect(8, 258, 41, 41);
  GUI_W_RadioImgAdd(&rec, G_DDS_PAUSE0, &pwm.run, 0);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_BTN_PAUSE);

  /*HOME button*/
  rec = GUI_Rect(190, 234, 41, 41);
  GUI_W_RadioImgAdd(&rec, G_DDS_BACK0, NULL, 0);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_BTN_HOME);

  /*main rotary button*/
  rec = GUI_Rect(69, 203, 102, 102);
  GUI_W_RotaryButtonAdd(&rec, &var8, ROTARY_BTN_GR_30_DEG);

  /*local init & jump to the handler*/

  GUI_SetUserTask(PWM_PageHandler);
}


/**
 * @function PWM_PageHandler
 * @brief PWM page handler
 * @param signal_t sig: signal coming from widgets
 * @return none
 */
static void PWM_PageHandler(signal_t sig) {

  /*handle user event*/
  switch(sig) {

    /*no signal? update registers*/
    case 0:
      PWM_Update(&pwm, false);
      break;

    /*frequency*/
    case SIG_RVAL_FREQ:
      LockValueBox();
      GUI_W_RotaryValueLock(pFreqVal, false);
      break;

    /*high period*/
    case SIG_RVAL_TH:
      LockValueBox();
      GUI_W_RotaryValueLock(pThVal, false);
      break;

    /*low period*/
    case SIG_RVAL_TL:
      LockValueBox();
      GUI_W_RotaryValueLock(pTlVal, false);
      break;

    /*duty cycle*/
    case SIG_RVAL_DUTY:
      LockValueBox();
      GUI_W_RotaryValueLock(pDutyVal, false);
      break;

    /*link th/tl*/
    case SIG_LINK_TL_TH:
      pwm.linkTlTh = !pwm.linkTlTh;
      GUI_W_ButtonShade(pBtnLink, !(pwm.linkTlTh));
      break;

    /*home*/
    case SIG_BTN_HOME:
      LocalExit();
      GUI_SetUserTask(GUI_MainMenu);
      break;

    /*PLAY button*/
    case SIG_BTN_RUN:
      PWM_Run(&pwm);
      break;

    /*PAUSE button*/
    case SIG_BTN_PAUSE:
      PWM_Stop(&pwm);
      break;

    default:
      break;
  }
}


/**
 * @function LocalInit
 * @brief initialize the PWM handler
 * @param none
 * @return none
 */
static void LocalInit(void) {
  static bool bFirstRun = true;
  PWM_Init(&pwm, bFirstRun);
  bFirstRun = false;
}


/**
 * @function LocalExit
 * @brief clean exit
 * @param none
 * @return none
 */
static void LocalExit(void) {
  PWM_Stop(&pwm);
}


/**
 * @function LockValueBox
 * @brief lock all valueboxes
 * @param none
 * @return none
 */
static void LockValueBox(void) {
  GUI_W_RotaryValueLock(pFreqVal, true);
  GUI_W_RotaryValueLock(pTlVal, true);
  GUI_W_RotaryValueLock(pThVal, true);
  GUI_W_RotaryValueLock(pDutyVal, true);
}
