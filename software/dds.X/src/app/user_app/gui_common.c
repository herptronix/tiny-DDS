/**
 * @file gui_common.c
 * @brief common graphics util / main menu
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

#include "usr_main.h"
#include "gui_common.h"
#include "dds_page.h"
#include "arb_page.h"
#include "pwm_page.h"


enum {
  SIG_PAGE_DDS = 1,
  SIG_PAGE_ARB,
  SIG_PAGE_PWM
};


/**
 * local functions
 */
static void GUI_MainMenuHandler(signal_t sig);


/**
 * @function DrawBackground
 * @brief add the background image in the current GUI page
 * @param none
 * @return none
 */
void DrawBackground(void) {
  rect_st rec;
  rec = GUI_Rect(0, 0, 240, 320);
  GUI_W_ImgAdd(&rec, G_IMG_BACKROUND, DISPLAY_SOLID);
}


/**
 * @function DrawIO
 * @brief draw I/O status
 * @param bool pwm, anaOut, anaIn: true -> enabled
 * @return none
 */
void DrawIO(bool pwm, bool anaOut, bool anaIn) {

  rect_st rec;

  /*in / out (PWM out / DDS out / ANA in)*/
  rec = GUI_Rect(65, 5, SpriteGetWidth(G_OUT_OFF), SpriteGetWidth(G_IN_OFF));

  GUI_W_ImgAdd(&rec, pwm? G_OUT_ON: G_OUT_OFF, DISPLAY_TRANSPARENT);
  rec.x += 70;
  GUI_W_ImgAdd(&rec, anaOut? G_OUT_ON : G_OUT_OFF, DISPLAY_TRANSPARENT);
  rec.x += 70;
  GUI_W_ImgAdd(&rec, anaIn? G_IN_ON: G_IN_OFF, DISPLAY_TRANSPARENT);
}


/**
 * @function GUI_MainMenu
 * @brief main menu page
 * @param signal_t sig: unused
 * @return none
 */
void GUI_MainMenu(signal_t sig) {

  rect_st rec;

  GUI_ClearAll();
  DrawBackground();

  /*logo*/
  rec.w = SpriteGetWidth(G_IMG_LOGO);
  rec.h = SpriteGetHeight(G_IMG_LOGO);
  rec.y = 120;
  rec.x = ((coord_t) LCD_GetWidth() - rec.w) / 2;
  GUI_W_ImgAdd(&rec, G_IMG_LOGO, DISPLAY_TRANSPARENT);


  SetFont(G_FONT_WAVE_SYMBOL);
  rec = GUI_Rect(40, 200, 50, 50);

  /*DDS button*/
  GUI_W_ButtonAdd(&rec, "\x9", 0);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_PAGE_DDS);

  /*PWM button*/
  rec.x += rec.w + 5;
  GUI_W_ButtonAdd(&rec, "\x3", 0);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_PAGE_PWM);

  /*Arbitrary button*/
  rec.x += rec.w + 5;
  GUI_W_ButtonAdd(&rec, "\xa", 0);
  GUI_SetSignal(E_PUSHED_TO_RELEASED, SIG_PAGE_ARB);

  SetFont(G_FONT_DEFAULT);
  GUI_SetUserTask(GUI_MainMenuHandler);
}


/**
 * @function GUI_MainMenuHandler
 * @brief GUI_MainMenu handler
 * @param signal_t sig: signal coming from widgets
 * @return none
 */
static void GUI_MainMenuHandler(signal_t sig) {

  switch(sig) {
    case SIG_PAGE_DDS:
      GUI_SetUserTask(DDS_Page);
      break;

    case SIG_PAGE_ARB:
      GUI_SetUserTask(ARB_Page);
      break;

    case SIG_PAGE_PWM:
       GUI_SetUserTask(PWM_Page);
      break;

    default:
      break;
  }
}
