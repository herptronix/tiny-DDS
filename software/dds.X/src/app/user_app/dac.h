/**
 * @file dac.h
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

#include "usr_main.h"

#ifndef _dac_h_
#define _dac_h_

/**
 * voltage descriptor
 */
typedef struct {
  int32_t vpp, oldVpp;
  int32_t offset, oldOffset;
  int32_t vmin, oldVmin;
  int32_t vmax, oldVmax;
  uint8_t channel;
} voltage_ctrl_st;


/**
 * @function DAC_Init
 * @brief initialize DAC & voltage_ctrl_st struct
 * @param voltage_ctrl_st *p: voltages descriptor
 * @param uint8_t channel: physical channel
 * @param bool bForceHwUpdate: if true, DAC will be force-updated
 * @return none
 */
void DAC_Init(voltage_ctrl_st *p, uint8_t channel, bool bFirstRun);

/**
 * @function DAC_Update
 * @brief updates voltages (Vpp, Voffset, Vmin & Vmax) according to the user input
 * @param voltage_ctrl_st *p: voltages
 * @param bool bForceHwUpdate: if true, DAC will be force-updated
 * @note voltages values are assumed to be coherent!
 * @return none
 */
void DAC_Update(voltage_ctrl_st *p, bool bForceHwUpdate);

/**
 * @function DAC_GetWordVpp
 * @brief compute the raw word corresponding to the given Vpp
 * @param int32_t vpp: Vpp voltage, in volt x 100
 * @note voltages values are assumed to be coherent!
 * @return none
 */
uint16_t DAC_GetWordVpp(int32_t vpp);

/**
 * @function DAC_GetWordVo
 * @brief compute the raw word corresponding to the given Vo
 * @param int32_t vo: Vo voltage, in volt x 100
 * @note voltages values are assumed to be coherent!
 * @return none
 */
uint16_t DAC_GetWordVo(int32_t vo);

#endif
