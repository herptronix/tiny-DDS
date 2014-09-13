/**
 * @file AD9834.h
 * @brief AD9834 driver (DDS chip)
 * @author Duboisset Philippe
 * @version 0.1b
 * @date (yyyy-mm-dd) 2014-04-05
 *
 * Copyright (C) <2013>  Duboisset Philippe <duboisset.philippe@gmail.com>
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

#ifndef _ad9834_h_
#define _ad9834_h_

#include "main.h"

enum {
  DDS_CH1,
  DDS_COUNT
};

typedef enum {
  SINUSOIDAL_WAVEFORM,
  TRIANGULAR_WAVEFORM
} waveform_e;

#ifdef ARCH_PIC32
/**
 * @function AD9834_Init
 * @brief initialize all listed DDS chips
 * @param none
 * @return none
 */
void AD9834_Init(void);

/**
 * @function AD9834_Clear
 * @brief clear all registers of a chip and hold it in RESET
 * @param uint8_t dev: chip #id ( < DDS_COUNT)
 * @return none
 */
void AD9834_Clear(uint8_t dev);

/**
 * @function AD9834_Suspend
 * @brief stop the internal clock; DAC remains at its current value
 * @param uint8_t dev: chip #id ( < DDS_COUNT)
 * @return none
 */
void AD9834_Suspend(uint8_t dev);

/**
 * @function AD9834_Stop
 * @brief hold in RESET
 * @param uint8_t dev: chip #id ( < DDS_COUNT)
 * @return none
 */
void AD9834_Stop(uint8_t dev);

/**
 * @function AD9834_Resume
 * @brief restart the internal clock
 * @param uint8_t dev: chip #id ( < DDS_COUNT)
 * @return none
 */
void AD9834_Resume(uint8_t dev);

/**
 * @function AD9834_SetWaveform
 * @brief select a waveform (triangle or sinus)
 * @param uint8_t dev: chip #id ( < DDS_COUNT)
 * @param waveform_e form: see waveform_e enum
 * @return none
 */
void AD9834_SetWaveform(uint8_t dev, waveform_e form);

/**
 * @function AD9834_SetFrequency
 * @brief set the output frequency
 * @param uint8_t dev: chip #id ( < DDS_COUNT)
 * @param int32_t freq: frequency (in Hz) x 10
 * @return none
 */
void AD9834_SetFrequency(uint8_t dev, int32_t freq);

/**
 * @function AD9834_SetPhase
 * @brief set the signal phase
 * @param uint8_t dev: chip #id ( < DDS_COUNT)
 * @param uint16_t word_12bits: phase (0=0°, 4095=360°)
 * @return none
 */
void AD9834_SetPhase(uint8_t dev, uint16_t word_12bits);

#else
  #define AD9834_Init()
  #define AD9834_Clear(dev)
  #define AD9834_SetWaveform(dev, form)
  #define AD9834_SetFrequency(dev, freq)
  #define AD9834_Suspend(dev)
  #define AD9834_Stop(dev)
  #define AD9834_Resume(dev)
  #define AD9834_SetPhase(dev, word_12bits)
#endif

#endif
