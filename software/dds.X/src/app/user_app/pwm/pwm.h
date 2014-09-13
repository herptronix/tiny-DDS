/**
 * @file pwm.h
 * @brief pwm output management
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

#ifndef _pwm_h_
#define _pwm_h_

#include "usr_main.h"


/**
 * pwm_ctrl_st
 * PWM handler
 */
typedef struct {

  /*displayed values*/
  int32_t freq, freqOld, freqMin, freqMax;  /*in Hz x 10*/
  int32_t duty, dutyOld, dutyMin, dutyMax;  /*between 0 - 1000*/
  int32_t th, thOld, thMin, thMax;          /*in ns*/
  int32_t tl, tlOld, tlMin, tlMax;          /*in ns*/

  /*hardware registers*/
  uint32_t tmrReg, tmrRegOld;
  uint32_t thReg, thRegOld;
  uint32_t tlReg, tlRegOld;

  int32_t resolution;
  uint8_t run, linkTlTh;
} pwm_ctrl_st;


/**
 * @function PWM_Init
 * @brief initialize the PWM handler
 * @param pwm_ctrl_st *p: pwm handler
 * @param bool bFirstRun: if true, the handler will be fully cleared
 * @return none
 */
void PWM_Init(pwm_ctrl_st *p, bool bFirstRun);

/**
 * @function PWM_Run
 * @brief start the PWM generation
 * @param pwm_ctrl_st *p: pwm handler
 * @return none
 */
void PWM_Run(pwm_ctrl_st *p);

/**
 * @function PWM_Stop
 * @brief stop the PWM generation
 * @param pwm_ctrl_st *p: pwm handler
 * @return none
 */
void PWM_Stop(pwm_ctrl_st *p);

/**
 * @function PWM_Update
 * @brief looks for parameters modifications, then update hardware registers
 * @param pwm_ctrl_st *p: pwm handler
 * @param bool bForceUpdate: force refresh register
 * @return none
 */
void PWM_Update(pwm_ctrl_st *p, bool bForceUpdate);

#endif
