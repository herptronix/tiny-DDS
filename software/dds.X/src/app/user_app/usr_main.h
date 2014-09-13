/**
 * @file usr_main.h
 * @brief embedded user app entry point
 * @author Duboisset Philippe
 * @version based on 0.1b (modification not checked) 2014-04-05
 * @date (yyyy-mm-dd) 2013-11-03
 *
 * Copyright (C) <2013>  Duboisset Philippe <duboisset.philippe@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _usr_main_h_
#define _usr_main_h_

#include "main.h"
#include "p2d.h"
#include "gui.h"
#include "gui_w.h"
#include "gui_m.h"
#include "resources.h"

/**
 * @function UserTask
 * @brief user process, executed at each software cycle
 * @param none
 * @return none
 */
void UserTask(void);

#endif
