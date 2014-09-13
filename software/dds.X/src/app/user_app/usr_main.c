/**
 * @file usr_main.c
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

#include "usr_main.h"
#include "dds.h"
#include "dac.h"
#include "spi.h"
#include "gui_common.h"
#include "wav_player.h"


/**
 * @function UserTask
 * @brief user process, executed at each software cycle
 * @param none
 * @return none
 */
void UserTask(void) {

  static bool bInitialized = false;

  /*first run?*/
  if(bInitialized == false) {

    /*hw init*/
    SPI_Init();
    DDS_Init(NULL, 0, true);
    DAC_Init(NULL, 0, true);

    /*GUI init*/
    GUI_ClearAll();
    GUI_SetUserTask(GUI_MainMenu);

    bInitialized = true;
  }

  /*wav file process is the only background process*/
  WavProcess();
}
