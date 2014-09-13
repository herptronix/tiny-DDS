/**
 * @file arb_out.h
 * @brief arbitrary waveform output
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

#ifndef _arb_out_h_
#define _arb_out_h_

#include "usr_main.h"

/**
 * @function OutputOnDdsDac
 * @brief output data on the internal DAC of the DDS
 * @param uint16_t val: mod input (from 0 to 0xFFFF)
 * @return none
 */
void OutputOnDdsDac(uint16_t val);

/**
 * @function ARB_SetOutputFreqMinMax
 * @brief define min & max frequence for FM
 * @param int32_t _fmin, int32_t _fmax: freq, in Hz x 10
 * @return none
 */
void ARB_SetOutputFreqMinMax(int32_t _fmin, int32_t _fmax);

/**
 * @function OutputOnDdsFreq
 * @brief modulate the frequency accroding to given val
 * @param uint16_t val: mod input (from 0 to 0xFFFF)
 * @return none
 */
void OutputOnDdsFreq(uint16_t val);

/**
 * @function ARB_SetOutputVppMinMax
 * @brief define min & max voltages for AM
 * @param int32_t _vmin, int32_t _vmax: voltages, in V x 100
 * @return none
 */
void ARB_SetOutputVppMinMax(int32_t _vmin, int32_t _vmax);

/**
 * @function OutputOnVpp
 * @brief output data on the Vpp DAC
 * @param uint16_t val: mod input (from 0 to 0xFFFF)
 * @return none
 */
void OutputOnVpp(uint16_t val);

/**
 * @function OutputOnVo
 * @brief output data on the Vo DAC
 * @param uint16_t val: mod input (from 0 to 0xFFFF)
 * @return none
 */
void OutputOnVo(uint16_t val);

#endif
