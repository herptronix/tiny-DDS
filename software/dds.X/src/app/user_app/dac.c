/**
 * @file dac.c
 * @brief voltages handler (Vpp, Vo, Vmin, Vmax)
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

#include "dac.h"
#include "ana.h"
#include "AD5310.h"


#define DAC_SetRawVpp(ch, raw)  AD5310_Set(ch * 2, raw)
#define DAC_SetRawVo(ch, raw)   AD5310_Set(ch * 2 + 1, raw)


/**
 * @function DAC_Init
 * @brief initialize DAC & voltage_ctrl_st struct
 * @param voltage_ctrl_st *p: voltages descriptor
 * @param uint8_t channel: physical channel
 * @param bool bFirstRun: if true, voltage_ctrl_st will be set to default
 * @return none
 */
void DAC_Init(voltage_ctrl_st *p, uint8_t channel, bool bFirstRun) {

  AD5310_Init();

  /*first run? clear all, set 5Vpp, no offset*/
  if(p != NULL && bFirstRun) {
    memset(p, 0, sizeof(*p));
    p->vpp = 500;
  }

  if(p != NULL) {
    p->channel = channel;
  }

  DAC_Update(p, true);
}


/**
 * @function DAC_Update
 * @brief updates voltages (Vpp, Voffset, Vmin & Vmax) according to the user input
 * @param voltage_ctrl_st *p: voltages
 * @param bool bForceHwUpdate: if true, DAC will be force-updated
 * @note voltages values are assumed to be coherent!
 * @return none
 */
void DAC_Update(voltage_ctrl_st *p, bool bForceHwUpdate) {

  int32_t max, absOffset;

  if(p != NULL) {

    /** vpp has been modified; align (if needed) offset*/
    if(p->oldVpp != p->vpp) {

      /*compute the maximal offset value according to the current vpp*/
      max = ANA_OUTPUT_MAX - (p->vpp / 2);

      /*offset greater than the maximal value? -> align it to the max*/
      absOffset = p->offset < 0? -p->offset: p->offset;
      if(absOffset > max) {
        if(p->offset < 0) p->offset = -max;
        else p->offset = max;
      }

      /*update vmin / vmax, force hardware update*/
      p->vmax = p->offset + (p->vpp / 2);
      p->vmin = p->offset - (p->vpp / 2);
      bForceHwUpdate = true;
    }


    /** offset has been modified; align (if needed) vpp*/
    else if(p->oldOffset != p->offset) {

      /*compute the maximal vpp value according to the current offset*/
      absOffset = p->offset < 0? -p->offset: p->offset;
      max = (ANA_OUTPUT_MAX - absOffset) * 2;

      /*vpp greater than the maximal value? -> align it to the max*/
      if(p->vpp > max) {
        p->vpp = max;
      }

      /*update vmin / vmax, force hardware update*/
      p->vmax = p->offset + (p->vpp / 2);
      p->vmin = p->offset - (p->vpp / 2);
      bForceHwUpdate = true;
    }


    /** Vmin has been modified; align offset & vpp*/
    else if(p->oldVmin != p->vmin) {
      if(p->vmin > p->offset - (ANA_OUTPUT_MIN_VPP / 2)) {
        p->vmin = p->offset - (ANA_OUTPUT_MIN_VPP / 2);
      }
      p->vpp = p->vmax - p->vmin;
      p->offset = p->vmin + (p->vpp / 2);
      bForceHwUpdate = true;
    }


    /** Vmax has been modified; align offset & vpp*/
    else if(p->oldVmax != p->vmax) {
      if(p->vmax < p->offset + (ANA_OUTPUT_MIN_VPP / 2)) {
        p->vmax = p->offset + (ANA_OUTPUT_MIN_VPP / 2);
      }
      p->vpp = p->vmax - p->vmin;
      p->offset = p->vmin + (p->vpp / 2);
      bForceHwUpdate = true;
    }


    /** store all current values*/
    p->oldVpp = p->vpp;
    p->oldOffset = p->offset;
    p->oldVmin = p->vmin;
    p->oldVmax = p->vmax;


    /** update registers*/
    if(bForceHwUpdate == true) {
      DAC_SetRawVpp(p->channel, DAC_GetWordVpp(p->vpp));
      DAC_SetRawVo(p->channel, DAC_GetWordVo(p->offset));
    }
  }
}


/**
 * @function DAC_GetWordVpp
 * @brief compute the raw word corresponding to the given Vpp
 * @param int32_t vpp: Vpp voltage, in volt x 100
 * @note voltages values are assumed to be coherent!
 * @return none
 */
uint16_t DAC_GetWordVpp(int32_t vpp) {

  static float coeff = (float)ANA_DAC_VPP_MAX_WORD / (2 * ANA_OUTPUT_MAX);
  int32_t word;

  /*set vpp*/
  word = ANA_DAC_MAX_WORD - (vpp * coeff);
  return word;
}


/**
 * @function DAC_GetWordVo
 * @brief compute the raw word corresponding to the given Vo
 * @param int32_t vo: Vo voltage, in volt x 100
 * @note voltages values are assumed to be coherent!
 * @return none
 */
uint16_t DAC_GetWordVo(int32_t vo) {

  static float coeff = (float)ANA_DAC_MAX_WORD / (2 * ANA_OUTPUT_MAX);
  int32_t word;

  /*set vo*/
  word = ANA_DAC_MAX_WORD - ((ANA_DAC_MAX_WORD / 2) + vo * coeff);
  return word;
}
