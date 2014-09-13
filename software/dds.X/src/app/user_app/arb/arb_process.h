/**
 * @file arb_process.h
 * @brief arbitrary waveform handler functions
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

#ifndef _arb_process_h_
#define _arb_process_h_

#include "usr_main.h"


#define ARB_WAVEFORM_DEPTH  201
#define ARB_TIMER           TMR_5

/*waveform type*/
typedef enum {
  ARB_WAVE_EMPTY,
  ARB_WAVE_TRIG,
  ARB_WAVE_PULSE,
  ARB_WAVE_RC,
  ARB_WAVE_POS_DSINE,
  ARB_WAVE_POS_HSINE,
  ARB_WAVE_NEG_HSINE,
  ARB_WAVE_NEG_DSINE,
  ARB_WAVE_SINE,
  ARB_WAVE_SINEXX,
  ARB_WAVE_NOISE,
  ARB_WAVE_WAV,
  ARB_WAVE_ANA_IN,
  _ARB_WAV_COUNT
} arb_waveform_e;

/*arb output*/
typedef enum {
  OUTPUT_ON_DDS_DAC,
  OUTPUT_ON_DDS_FREQ,
  OUTPUT_ON_VPP,
  OUTPUT_ON_VO
} arb_out_e;

/**
 * struct arb_st
 */
typedef struct {
  int32_t frequency, frequencyOld;
  void (*pOut) (uint16_t dataIn);
  volatile uint8_t currentSample;
  uint8_t sampleIncrement, run, waveform[ARB_WAVEFORM_DEPTH];
  arb_waveform_e waveformType;
} arb_st;

/**
 * @function ARB_Init
 * @brief initialize the arbitrary waveform handler
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param bool bFirstRun: clear the whole struct if true
 * @return none
 */
void ARB_Init(arb_st *arb, bool bFirstRun);

/**
 * @function ARB_Stop
 * @brief stop the arbitrary handler (stop timer, clear current sample #id)
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @return none
 */
void ARB_Stop(arb_st *arb);

/**
 * @function ARB_Pause
 * @brief suspend the arbitrary handler (timer pause)
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @return none
 */
void ARB_Pause(arb_st *arb);

/**
 * @function ARB_Run
 * @brief start / resume the arbitrary handler
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @return none
 */
void ARB_Run(arb_st *arb);

/**
 * @function ARB_UpdateFrequency
 * @brief update the output frequency of a waveform
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param bool bForceRefresh: force register write if true
 * @return none
 */
void ARB_UpdateFrequency(arb_st *arb, bool bForceRefresh);

/**
 * @function ARB_SetOutput
 * @brief select the output of the arb handler (to Vpp, Vo, Freq, DDS_dac)
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param arb_out_e out: see arb_out_e enum
 * @return none
 */
void ARB_SetOutput(arb_st *arb, arb_out_e out);

/**
 * @function ARB_SetWaveform
 * @brief set a waveform
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param arb_waveform_e waveformType: see arb_waveform_e enum
 * @return none
 */
void ARB_SetWaveform(arb_st *arb, arb_waveform_e waveformType);

/**
 * @function ARB_UpdateWaveform
 * @brief update the samples of a waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return
 */
bool ARB_UpdateWaveform(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1);

#endif
