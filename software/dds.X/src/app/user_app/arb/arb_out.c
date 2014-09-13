/**
 * @file arb_out.c
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

#include "arb_out.h"
#include "AD9834.h"
#include "AD5310.h"
#include "spi.h"
#include "dac.h"


/**
 * local variables
 */
static int32_t fmin, fmax;
static int32_t vmin, vmax;


/**
 * @function OutputOnDdsDac
 * @brief output data on the internal DAC of the DDS
 * @param uint16_t val: mod input (from 0 to 0xFFFF)
 * @return none
 */
void OutputOnDdsDac(uint16_t val) {
  if(IsSemUnlocked(spiBusy)) {
    /*set the phase corresponding to the sample #id; >> 5: 1/2 period is E[0;2048] whereas sample is E[0-65535]*/
    /*phase from 0 to 1023: triangle goes from 0 to +Vmax*/
    /*phase from 0 to 3071: triangle goes from +Vmax to -Vmax*/
    /*phase from 3072 to 4095: triangle goes from -Vmax to 0*/
    AD9834_SetPhase(0, 1024 + (val >> 5));
  }
}


/**
 * @function ARB_SetOutputFreqMinMax
 * @brief define min & max frequence for FM
 * @param int32_t _fmin, int32_t _fmax: freq, in Hz x 10
 * @return none
 */
void ARB_SetOutputFreqMinMax(int32_t _fmin, int32_t _fmax) {
  fmin = _fmin;
  fmax = _fmax;
}


/**
 * @function OutputOnDdsFreq
 * @brief modulate the frequency accroding to given val
 * @param uint16_t val: mod input (from 0 to 0xFFFF)
 * @return none
 */
void OutputOnDdsFreq(uint16_t val) {
  int64_t tmp;
  if(IsSemUnlocked(spiBusy)) {
    tmp = ((int64_t)fmax - fmin) * val / 0xFFFF + fmin;
    AD9834_SetFrequency(0, (int32_t)tmp);
  }
}


/**
 * @function ARB_SetOutputVppMinMax
 * @brief define min & max voltages for AM
 * @param int32_t _vmin, int32_t _vmax: voltages, in V x 100
 * @return none
 */
void ARB_SetOutputVppMinMax(int32_t _vmin, int32_t _vmax) {
  vmin = _vmin;
  vmax = _vmax;
}


/**
 * @function OutputOnVpp
 * @brief output data on the Vpp DAC
 * @param uint16_t val: mod input (from 0 to 0xFFFF)
 * @return none
 */
void OutputOnVpp(uint16_t val) {
  int32_t v;
  if(IsSemUnlocked(spiBusy)) {
    v = vmin + ((vmax - vmin) * val / 0xFFFF);
    AD5310_Set(0, DAC_GetWordVpp(v));
  }
}


/**
 * @function OutputOnVo
 * @brief output data on the Vo DAC
 * @param uint16_t val: mod input (from 0 to 0xFFFF)
 * @return none
 */
void OutputOnVo(uint16_t val) {
  if(IsSemUnlocked(spiBusy)) {
    AD5310_Set(1, val >> 6);
  }
}
