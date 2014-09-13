/**
 * @file dds.h
 * @brief DDS handler
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

#ifndef _dds_h_
#define _dds_h_

#include "usr_main.h"

typedef struct {
  int32_t offsetMin, offsetMax;
  int32_t vppMin, vppMax;
  int32_t freqMin, freqMax;
} ana_limits_st;

typedef struct {
  int32_t frequency, oldFrequency;
  uint16_t phase;
  uint8_t wave;
  uint8_t run;
  uint8_t channel;
  ana_limits_st lim;
} dds_ctrl_st;

enum {
  DDS_WAVE_SINUS,
  DDS_WAVE_TRIG,
  DDS_WAVE_DC
};

/**
 * @function DDS_Init
 * @brief initialize DDS & dds_ctrl_st struct
 * @param dds_ctrl_st *p: dds descriptor
 * @param uint8_t channel: physical channel
 * @param bool bFirstRun: if true, dds_ctrl_st will be set to default
 * @return none
 */
void DDS_Init(dds_ctrl_st *p, uint8_t channel, bool bFirstRun);

/**
 * @function DDS_Stop
 * @brief hold in RESET
 * @param dds_ctrl_st *p: dds descriptor
 * @return none
 */
void DDS_Stop(dds_ctrl_st *p);

/**
 * @function DDS_Pause
 * @brief stop the interal clock
 * @param dds_ctrl_st *p: dds descriptor
 * @return none
 */
void DDS_Pause(dds_ctrl_st *p);

/**
 * @function DDS_Run
 * @brief restart the internal clock
 * @param dds_ctrl_st *p: dds descriptor
 * @return none
 */
void DDS_Run(dds_ctrl_st *p);

/**
 * @function DDS_FequencyUpdate
 * @brief check if the freq. has changed; update register if needed
 * @param dds_ctrl_st *p: dds descriptor
 * @param bool bForceHwUpdate: force register update
 * @note: DDS_SetWave() shall be called first
 * @return none
 */
void DDS_FequencyUpdate(dds_ctrl_st *p, bool bForceHwUpdate);

/**
 * @function DDS_SetPhase
 * @brief set the signal phase
 * @param dds_ctrl_st *p: dds descriptor
 * @param uint16_t phase: 0=0°  --  4095=360°
 * @return none
 */
void DDS_SetPhase(dds_ctrl_st *p, uint16_t phase);

/**
 * @function DDS_SetWave
 * @brief set the waveform (sinus / triangle / dc)
 * @param dds_ctrl_st *p: dds descriptor
 * @param uint8_t w: DDS_WAVE_DC / DDS_WAVE_TRIG / DDS_WAVE_SINUS
 * @return none
 */
void DDS_SetWave(dds_ctrl_st *p, uint8_t w);

#endif
