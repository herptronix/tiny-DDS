/**
 * @file AD5310.h
 * @brief AD5310 driver (DAC)
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

#ifndef _ad5310_h_
#define _ad5310_h_

#include "main.h"


/**
 * enumeration of DAC
 */
enum {
  DAC_AMPLITUDE_CTRL,
  DAC_OFFSET_CRTL,
  DAC_COUNT
};

#ifdef ARCH_PIC32
/**
 * @function AD5310_Init
 * @brief init. all DAC (GPIO init & set to mid scale)
 * @param none
 * @return none
 */
void AD5310_Init(void);

/**
 * @function AD5310_Set
 * @brief Set a new binary word to the selected DAC
 * @param uint8_t dev: #id of the DAC
 * @param uint16_t word_10bits: new word (only the 10LSB are used)
 * @return none
 */
void AD5310_Set(uint8_t dev, uint16_t word_10bits);

#else
  #define AD5310_Init()
  #define AD5310_Set(dev, word_10bits)
#endif

#endif
