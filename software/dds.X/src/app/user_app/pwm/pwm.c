/**
 * @file pwm.c
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

#include "pwm.h"
#include <math.h>


/**
 * local variables
 */
static int32_t clock = 80000000;
static const uint32_t reg_max = 0xFFFFFFFF;


/**
 * local functions
 */
static void SetFrequency(pwm_ctrl_st *p);
static void SetTh(pwm_ctrl_st *p);
static void SetTl(pwm_ctrl_st *p);
static void SetDuty(pwm_ctrl_st *p);
static void UpdateDuty(pwm_ctrl_st *p);
static void UpdateResolution(pwm_ctrl_st *p);
static void UpdateFreq(pwm_ctrl_st *p);
static void UpdateTlTh(pwm_ctrl_st *p);


/**
 * @function PWM_Init
 * @brief initialize the PWM handler
 * @param pwm_ctrl_st *p: pwm handler
 * @param bool bFirstRun: if true, the handler will be fully cleared
 * @return none
 */
void PWM_Init(pwm_ctrl_st *p, bool bFirstRun) {

  if(bFirstRun == true && p != NULL) {

    memset(p, 0, sizeof(*p));

    /**hardware limitations*/
    p->dutyMin = 0;
    p->dutyMax = 1000;

    /*in Hz x 10*/
    p->freqMin = 5;
    p->freqMax = 10000000;

    /*in ns*/
    p->thMin = 0;
    p->thMax = 1000000000;
    p->tlMin = 0;
    p->tlMax = 1000000000;

    /*default: 100Hz, duty = 0.5, TH linked with TL*/
    p->duty = 500;
    p->freq = 1000;
    p->linkTlTh = 0;
  }

  PWM_Update(p, true);
}


/**
 * @function PWM_Run
 * @brief start the PWM generation
 * @param pwm_ctrl_st *p: pwm handler
 * @return none
 */
void PWM_Run(pwm_ctrl_st *p) {

  if(p != NULL && p->run == 0) {
    p->run = 1;
    #ifdef ARCH_PIC32
    OpenTimer23(T23_ON | T23_PS_1_1, p->tmrReg);
    OpenOC2(OC_ON | OC_TIMER2_SRC | OC_TIMER_MODE32 | OC_PWM_FAULT_PIN_DISABLE, p->thReg, 0);
    #endif
  }
}


/**
 * @function PWM_Stop
 * @brief stop the PWM generation
 * @param pwm_ctrl_st *p: pwm handler
 * @return none
 */
void PWM_Stop(pwm_ctrl_st *p) {
  #ifdef ARCH_PIC32
  CloseTimer23();
  #endif
  p->run = 0;
}


/**
 * @function PWM_Update
 * @brief looks for parameters modifications, then update hardware registers
 * @param pwm_ctrl_st *p: pwm handler
 * @param bool bForceUpdate: force refresh register
 * @return none
 */
void PWM_Update(pwm_ctrl_st *p, bool bForceUpdate) {

  if(p != NULL) {

    /*frequency modification*/
    if(p->freq != p->freqOld || bForceUpdate) {
      SetFrequency(p);
      SetDuty(p);
    }

    /*duty cycle modification*/
    else if(p->duty != p->dutyOld) {
      SetDuty(p);
    }

    /*TH modification*/
    else if(p->th != p->thOld) {
      SetTh(p);
      UpdateFreq(p);
      UpdateDuty(p);
    }

    /*TL modification*/
    else if(p->tl != p->tlOld) {
      SetTl(p);
      UpdateFreq(p);
      UpdateDuty(p);
    }

    /*register refresh*/
    if(p->run) {
      if(p->tmrRegOld != p->tmrReg) {
        #ifdef ARCH_PIC32
        PR2 = p->tmrReg;
        if(TMR2 >= p->tmrReg) TMR2 = 0;
        #endif
      }
      if(p->thRegOld != p->thReg || p->tlRegOld != p->tlReg) {
        #ifdef ARCH_PIC32
        SetDCOC2PWM(p->thReg);
        #endif
      }
    }

    /*compute actual values & save all*/
    UpdateTlTh(p);
    UpdateResolution(p);
    p->dutyOld = p->duty;
    p->freqOld = p->freq;
    p->thOld = p->th;
    p->tlOld = p->tl;
    p->tmrRegOld = p->tmrReg;
    p->thRegOld = p->thReg;
    p->tlRegOld = p->tlReg;
  }
}


/**
 * @function SetFrequency
 * @brief update registers according to new frequency value
 * @param pwm_ctrl_st *p: pwm handler
 * @return none
 */
static void SetFrequency(pwm_ctrl_st *p) {

  /*get the closest TMR value according to the desired freq*/
  p->tmrReg = clock * 10 / p->freq;

  /*the user increment is too small -> direclty +1/-1 the TMR*/
  if(p->tmrReg == p->tmrRegOld) {
    if(p->freq > p->freqOld && p->tmrReg > 1) p->tmrReg--;
    else if(p->tmrReg < reg_max) p->tmrReg++;
  }

  /*compute the final "real" frequency, based on TMR register*/
  p->freq = clock * 10 / p->tmrReg;
}


/**
 * @function SetTh
 * @brief update registers according to new TH value
 * @param pwm_ctrl_st *p: pwm handler
 * @return none
 */
static void SetTh(pwm_ctrl_st *p) {

  uint64_t tmp;

  /*compute the th register value, according to user input*/
  tmp = (uint64_t)p->th * clock / 1e9;
  if(tmp > reg_max) tmp = reg_max;
  p->thReg = (uint32_t) tmp;

  /*the user increment is too small -> direclty +1/-1 the TH reg*/
  if(p->thReg == p->thRegOld) {
    if(p->th < p->thOld && p->thReg > 0) p->thReg--;
    else if(p->thReg < reg_max) p->thReg++;
  }

  /*linked with TL?*/
  if(p->linkTlTh) {
    /*ensure that th <= treg, then deduce tl*/
    if(p->thReg > p->tmrReg) p->thReg = p->tmrReg;
    p->tlReg = p->tmrReg - p->thReg;
  }
  else {
    /*ensure that th+tl < regmax && th+tl > 0*/
    if((uint64_t)p->thReg + p->tlReg > reg_max) p->tlReg = reg_max - p->thReg;
    else if((uint64_t)p->thReg + p->tlReg == 0) p->thReg = 1;
  }
}


/**
 * @function SetTl
 * @brief update registers according to new TL value
 * @param pwm_ctrl_st *p: pwm handler
 * @return none
 */
static void SetTl(pwm_ctrl_st *p) {

  uint64_t tmp;

  /*compute the th register value, according to user input*/
  tmp = (uint64_t)p->tl * clock / 1e9;
  if(tmp > reg_max) tmp = reg_max;
  p->tlReg = (uint32_t) tmp;

  /*the user increment is too small -> direclty +1/-1 the TL reg*/
  if(p->tlReg == p->tlRegOld) {
    if(p->tl < p->tlOld && p->tlReg > 0) p->tlReg--;
    else if(p->tlReg < reg_max) p->tlReg++;
  }

  /*linked with TL?*/
  if(p->linkTlTh) {
    /*ensure that tl <= treg, then deduce th*/
    if(p->tlReg > p->tmrReg) p->tlReg = p->tmrReg;
    p->thReg = p->tmrReg - p->tlReg;
  }
  else {
    /*ensure that th+tl < regmax && th+tl > 0*/
    if((uint64_t)p->tlReg + p->thReg> reg_max) p->thReg = reg_max - p->tlReg;
    else if((uint64_t)p->thReg + p->tlReg == 0) p->tlReg = 1;
  }
}


/**
 * @function SetDuty
 * @brief update registers according to new duty cycle value
 * @param pwm_ctrl_st *p: pwm handler
 * @return none
 */
static void SetDuty(pwm_ctrl_st *p) {
  uint64_t tmp;
  tmp = (uint64_t)p->duty * p->tmrReg / 1000; /*duty is between 0-1000*/
  p->thReg = (uint32_t)tmp;
  p->tlReg = p->tmrReg - p->thReg;
}


/**
 * @function UpdateDuty
 * @brief compute the actual duty cycle value
 * @param pwm_ctrl_st *p: pwm handler
 * @return none
 */
static void UpdateDuty(pwm_ctrl_st *p) {
  if(p->tmrReg != 0) {  /*should not happen*/
    p->duty = (uint32_t) ((uint64_t) p->thReg * 1000 / p->tmrReg);
  }
}


/**
 * @function UpdateResolution
 * @brief compute the corresponding resolution, in bits
 * @param pwm_ctrl_st *p: pwm handler
 * @return none
 */
static void UpdateResolution(pwm_ctrl_st *p) {
  float tmp;
  if(p->freq != 0) {  /*should not happen*/
    tmp = log10(10.0f * clock / p->freq ) / log10(2);
    p->resolution = (int32_t) (tmp * 10);
  }
}


/**
 * @function UpdateFreq
 * @brief compute the acutal frequency value
 * @param pwm_ctrl_st *p: pwm handler
 * @return none
 */
static void UpdateFreq(pwm_ctrl_st *p) {
  if(p->tmrReg != 0) {  /*should not happen*/
    p->tmrReg = p->thReg + p->tlReg;
    p->freq = (uint32_t)((uint64_t) clock * 10 / p->tmrReg);
  }
}


/**
 * @function UpdateTlTh
 * @brief compute the actual TH / TL values
 * @param pwm_ctrl_st *p: pwm handler
 * @return none
 */
static void UpdateTlTh(pwm_ctrl_st *p) {
  uint64_t tmp;
  tmp = (uint64_t)p->thReg * 1e9 / clock;
  p->th = (uint32_t)tmp;
  tmp = (uint64_t)p->tlReg * 1e9 / clock;
  p->tl = (uint32_t)tmp;
}
