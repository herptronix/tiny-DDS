/**
 * @file dds.c
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

#include "dds.h"
#include "ana.h"
#include "AD9834.h"


#ifndef SMART_TFT_SLAVE_MODE


/**
 * @function DDS_Init
 * @brief initialize DDS & dds_ctrl_st struct
 * @param dds_ctrl_st *p: dds descriptor
 * @param uint8_t channel: physical channel
 * @param bool bFirstRun: if true, dds_ctrl_st will be set to default
 * @return none
 */
void DDS_Init(dds_ctrl_st *p, uint8_t channel, bool bFirstRun) {

  AD9834_Init();

  /*first run? clear all, sine @1kHz*/
  if(p != NULL && bFirstRun) {
    memset(p, 0, sizeof(*p));
    p->frequency = 10000;
    p->wave = DDS_WAVE_SINUS;
  }

  if(p != NULL) {
    p->channel = channel;
    DDS_Stop(p);
    DDS_SetWave(p, p->wave);
    DDS_FequencyUpdate(p, bFirstRun);
  }
}


/**
 * @function DDS_Stop
 * @brief hold in RESET
 * @param dds_ctrl_st *p: dds descriptor
 * @return none
 */
void DDS_Stop(dds_ctrl_st *p) {
  if(p != NULL) {
    p->run = 0;
    AD9834_Stop(p->channel);
  }
}


/**
 * @function DDS_Pause
 * @brief stop the interal clock
 * @param dds_ctrl_st *p: dds descriptor
 * @return none
 */
void DDS_Pause(dds_ctrl_st *p) {
  if(p != NULL) {
    p->run = 0;
    AD9834_Suspend(p->channel);
  }
}


/**
 * @function DDS_Run
 * @brief restart the internal clock
 * @param dds_ctrl_st *p: dds descriptor
 * @return none
 */
void DDS_Run(dds_ctrl_st *p) {
  if(p != NULL) {
    if(p->wave != DDS_WAVE_DC) {
      p->run = 1;
      AD9834_Resume(p->channel);
    }
  }
}


/**
 * @function DDS_FequencyUpdate
 * @brief check if the freq. has changed; update register if needed
 * @param dds_ctrl_st *p: dds descriptor
 * @param bool bForceHwUpdate: force register update
 * @note: DDS_SetWave() shall be called first
 * @return none
 */
void DDS_FequencyUpdate(dds_ctrl_st *p, bool bForceHwUpdate) {

  if(p != NULL) {

    /*ensure that the frequency is correct*/
    if(p->frequency < p->lim.freqMin) {
      p->frequency = p->lim.freqMin;
    }
    else if(p->frequency > p->lim.freqMax) {
      p->frequency = p->lim.freqMax;
    }

    /*compare with the old one*/
    if(p->oldFrequency != p->frequency) {
      p->oldFrequency = p->frequency;
      bForceHwUpdate = true;
    }

    /*update registers, if needed*/
    if(bForceHwUpdate) {
      AD9834_SetFrequency(p->channel, p->frequency);
    }
  }
}


/**
 * @function DDS_SetPhase
 * @brief set the signal phase
 * @param dds_ctrl_st *p: dds descriptor
 * @param uint16_t phase: 0=0°  --  4095=360°
 * @return none
 */
void DDS_SetPhase(dds_ctrl_st *p, uint16_t phase) {
  if(p != NULL) {
    AD9834_SetPhase(p->channel, phase);
  }
}


/**
 * @function DDS_SetWave
 * @brief set the waveform (sinus / triangle / dc)
 * @param dds_ctrl_st *p: dds descriptor
 * @param uint8_t w: DDS_WAVE_DC / DDS_WAVE_TRIG / DDS_WAVE_SINUS
 * @return none
 */
void DDS_SetWave(dds_ctrl_st *p, uint8_t w) {

  if(p != NULL) {
    switch(w) {

      case DDS_WAVE_DC:

        /*stop the phase counter & set DDS output to 0V in DC mode*/
        p->wave = DDS_WAVE_DC;
        DDS_Stop(p);

        /*analog limitations*/
        p->lim.offsetMax = ANA_OUTPUT_MAX;
        p->lim.offsetMin = -p->lim.offsetMax;
        p->lim.vppMin = 0;
        p->lim.vppMax = 0;
        p->lim.freqMin = ANA_FREQ_MIN;
        p->lim.freqMax = ANA_FREQ_SINE_MAX;
        break;

      case DDS_WAVE_TRIG:

        /*triangle waveform*/
        p->wave = DDS_WAVE_TRIG;
        AD9834_SetWaveform(p->channel, TRIANGULAR_WAVEFORM);

        /*analog limitations*/
        p->lim.offsetMax = ANA_OUTPUT_MAX - (ANA_OUTPUT_MIN_VPP / 2);
        p->lim.offsetMin = -p->lim.offsetMax;
        p->lim.vppMin = ANA_OUTPUT_MIN_VPP;
        p->lim.vppMax = 2 * ANA_OUTPUT_MAX;
        p->lim.freqMin = ANA_FREQ_MIN;
        p->lim.freqMax = ANA_FREQ_TRIG_MAX;
        break;

      case DDS_WAVE_SINUS:
      default:
        /*sinus waveform*/
        p->wave = DDS_WAVE_SINUS;
        AD9834_SetWaveform(p->channel, SINUSOIDAL_WAVEFORM);

        /*analog limitations*/
        p->lim.offsetMax = ANA_OUTPUT_MAX - (ANA_OUTPUT_MIN_VPP / 2);
        p->lim.offsetMin = -p->lim.offsetMax;
        p->lim.vppMin = ANA_OUTPUT_MIN_VPP;
        p->lim.vppMax = 2 * ANA_OUTPUT_MAX;
        p->lim.freqMin = ANA_FREQ_MIN;
        p->lim.freqMax = ANA_FREQ_SINE_MAX;
        break;
    }
  }
}

#endif
