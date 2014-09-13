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

#ifndef _gui_common_h_
#define _gui_common_h_

#include "usr_main.h"


/**
 * @function DrawBackground
 * @brief add the background image in the current GUI page
 * @param none
 * @return none
 */
void DrawBackground(void);

/**
 * @function DrawIO
 * @brief draw I/O status
 * @param bool pwm, anaOut, anaIn: true -> enabled
 * @return none
 */
void DrawIO(bool pwm, bool anaOut, bool anaIn);

/**
 * @function GUI_MainMenu
 * @brief main menu page
 * @param signal_t sig: unused
 * @return none
 */
void GUI_MainMenu(signal_t sig);

#endif
