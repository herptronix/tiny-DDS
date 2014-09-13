/**
 * @file arb_process.c
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

#include "AD9834.h"
#include "ana.h"
#include "arb_out.h"
#include "arb_process.h"
#include "arb_wavedraw.h"
#include "mod.h"
#include "P2D.h"
#include "tmr.h"
#include "wav_player.h"


/**
 * local variables
 */
static arb_st *currentArb = NULL;


/**
 * local functions
 */
static void ARB_IsrStd(void);
static void ARB_IsrNoise(void);
static void ARB_IsrAnaIn(void);


/**
 * @function ARB_Init
 * @brief initialize the arbitrary waveform handler
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param bool bFirstRun: clear the whole struct if true
 * @return none
 */
void ARB_Init(arb_st *arb, bool bFirstRun) {

  if(arb != NULL) {

    /**default config: 100Hz, triangle, modulation OFF*/
    if(bFirstRun) {
      memset(arb, 0, sizeof(arb_st));
      ARB_SetWaveform(arb, ARB_WAVE_TRIG);
      arb->frequency = 1000;
      ARB_UpdateFrequency(arb, true);
    }

    /*stop ARB process*/
    ARB_Stop(arb);

    /**configure DDS & output stream*/
    if(modType == MOD_OFF) {
      ARB_SetOutput(arb, OUTPUT_ON_DDS_DAC);
      AD9834_SetWaveform(0, DDS_WAVE_TRIG);
      AD9834_SetFrequency(0, 0);
    }
    else if(modType == MOD_AM) {
      ARB_SetOutputVppMinMax(modAM.vppMin, modAM.vppMax);
      ARB_SetOutput(arb, OUTPUT_ON_VPP);
      AD9834_SetWaveform(0, DDS_WAVE_SINUS);
      AD9834_SetFrequency(0, modAM.frequency);
    }
    else {
      ARB_SetOutput(arb, OUTPUT_ON_DDS_FREQ);
      ARB_SetOutputFreqMinMax(modFM.freqMin, modFM.freqMax);
      AD9834_SetFrequency(0, modFM.freqMin);
      AD9834_SetWaveform(0, DDS_WAVE_SINUS);
    }
  }
}


/**
 * @function ARB_Stop
 * @brief stop the arbitrary handler (stop timer, clear current sample #id)
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @return none
 */
void ARB_Stop(arb_st *arb) {

  if(arb != NULL) {
    arb->run = 0;
    arb->currentSample = 0;

    /*stop the timer first, then the DDS*/
    TmrStop(ARB_TIMER);
    AD9834_Stop(0);

    /*kill the wav playback if any*/
    if(arb->waveformType == ARB_WAVE_WAV) {
      WavStop();
    }
  }
}


/**
 * @function ARB_Pause
 * @brief suspend the arbitrary handler (timer pause)
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @return none
 */
void ARB_Pause(arb_st *arb) {
  if(arb != NULL) {
    arb->run = 0;
    TmrStop(ARB_TIMER);
  }
}


/**
 * @function ARB_Run
 * @brief start / resume the arbitrary handler
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @return none
 */
void ARB_Run(arb_st *arb) {

  if(arb != NULL) {
    arb->run = 1;
    currentArb = arb;

    /*DDS on*/
    AD9834_Resume(0);

    switch(arb->waveformType) {

      /*<noise> special case: overwrite frequency with MAX_SAMPLE_PER_SECOND*/
      case ARB_WAVE_NOISE:
        TmrSetCallback(ARB_TIMER, ARB_IsrNoise);
        TmrSetFrequency(ARB_TIMER, MAX_SAMPLE_PER_SECOND);
        TmrLaunch(ARB_TIMER);
        break;

      /*<wav> special case: timer & ISR are directly handled by OpenWav()*/
      case ARB_WAVE_WAV:
        if(WavGetStatus() == WAV_STOPPED) {
          OpenWav("waveform.wav");
        }
        WavPlay(arb->pOut);
        break;

      /*analog in: special ISR*/
      case ARB_WAVE_ANA_IN:
        TmrSetCallback(ARB_TIMER, ARB_IsrAnaIn);
        TmrLaunch(ARB_TIMER);
        break;

      /*for all other waveforms: just start the timer with the default ISR*/
      default:
        TmrSetCallback(ARB_TIMER, ARB_IsrStd);
        TmrLaunch(ARB_TIMER);
        break;
    }
  }
}


/**
 * @function ARB_UpdateFrequency
 * @brief update the output frequency of a waveform
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param int32_t freq: frequency, in Hz x 10
 * @param bool bForceRefresh: force register write if true
 * @return none
 */
void ARB_UpdateFrequency(arb_st *arb, bool bForceRefresh) {

  float f;
  uint32_t maxSamplePerSec = MAX_SAMPLE_PER_SECOND;

  if(arb != NULL) {
    if(arb->frequencyOld != arb->frequency || bForceRefresh) {

      arb->frequencyOld = arb->frequency;

      /*AM/FM modulation ISRs need much more time -> decrease the sampling rate*/
      if(modType != MOD_OFF) maxSamplePerSec /= 10;

      /*if the frequency is too high, increase the sample increment*/
      arb->sampleIncrement = (arb->frequency * ARB_WAVEFORM_DEPTH / 10) / maxSamplePerSec;
      if(arb->sampleIncrement == 0) arb->sampleIncrement = 1;

      f = (((float)arb->frequency / 10) * ARB_WAVEFORM_DEPTH / arb->sampleIncrement);
      TmrSetFrequency(ARB_TIMER, f);

      /*re-launch the timer if needed (changing the freq through TmrSetFrequency() stop it)*/
      if(arb->run) ARB_Run(arb);
    }
  }
}


/**
 * @function ARB_SetOutput
 * @brief select the output of the arb handler (to Vpp, Vo, Freq, DDS_dac)
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param arb_out_e out: see arb_out_e enum
 * @return none
 */
void ARB_SetOutput(arb_st *arb, arb_out_e out) {
  if(arb != NULL) {
    if(out == OUTPUT_ON_DDS_FREQ) arb->pOut = OutputOnDdsFreq;
    else if(out == OUTPUT_ON_VPP) arb->pOut = OutputOnVpp;
    else if(out == OUTPUT_ON_VO)  arb->pOut = OutputOnVo;
    else arb->pOut = OutputOnDdsDac;
  }
}


/**
 * @function ARB_SetWaveform
 * @brief set a waveform type
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param arb_waveform_e waveformType: see arb_waveform_e enum
 * @return none
 */
void ARB_SetWaveform(arb_st *arb, arb_waveform_e waveformType) {

  if(arb != NULL) {

    /*stop ISR, wav playback & co*/
    ARB_Stop(arb);

    switch(waveformType) {

      case ARB_WAVE_TRIG:       ARB_UpdateWaveformTriangle(arb, 0, 0, 0, 30, 0); break;
      case ARB_WAVE_PULSE:      ARB_UpdateWaveformPulse   (arb, 0, 0, 0, 30, 0); break;
      case ARB_WAVE_RC:         ARB_UpdateWaveformRC      (arb, 0, 0, 0, 30, 0); break;
      case ARB_WAVE_POS_DSINE:  ARB_UpdateWaveformPosDSine(arb, 0, 0, 0, 30, 0); break;
      case ARB_WAVE_POS_HSINE:  ARB_UpdateWaveformPosHSine(arb, 0, 0, 0, 30, 0); break;
      case ARB_WAVE_NEG_HSINE:  ARB_UpdateWaveformNegHSine(arb, 0, 0, 0, 30, 0); break;
      case ARB_WAVE_NEG_DSINE:  ARB_UpdateWaveformNegDSine(arb, 0, 0, 0, 30, 0); break;
      case ARB_WAVE_SINEXX:     ARB_UpdateWaveformSineXX  (arb, 0, 0, 0, 30, 0); break;
      case ARB_WAVE_SINE:       ARB_UpdateWaveformSine    (arb, 0, 0, 0, 30, 0); break;
      case ARB_WAVE_ANA_IN:     memset(arb->waveform, 127, ARB_WAVEFORM_DEPTH) ; break;

      /*nothing to do here for these waveforms*/
      case ARB_WAVE_WAV:
      case ARB_WAVE_NOISE:
        break;

      /*unknown waveform -> clear the sample table*/
      default:
        waveformType = ARB_WAVE_EMPTY;
        memset(arb->waveform, 127, ARB_WAVEFORM_DEPTH);
        break;
    }

    /*save waveform & refresh frequency*/
    arb->waveformType = waveformType;
    ARB_UpdateFrequency(arb, true);
  }
}


/**
 * @function ARB_UpdateWaveform
 * @brief update the samples of a waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return
 */
bool ARB_UpdateWaveform(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1) {

  bool bRefresh = false;

  if(arb != NULL) {
    switch(arb->waveformType) {

      case ARB_WAVE_TRIG:   bRefresh = ARB_UpdateWaveformTriangle(arb, graphHeight, x0, y0, x1, y1); break;
      case ARB_WAVE_PULSE:  bRefresh = ARB_UpdateWaveformPulse   (arb, graphHeight, x0, y0, x1, y1); break;
      case ARB_WAVE_RC:     bRefresh = ARB_UpdateWaveformRC      (arb, graphHeight, x0, y0, x1, y1); break;
      case ARB_WAVE_SINEXX: bRefresh = ARB_UpdateWaveformSineXX  (arb, graphHeight, x0, y0, x1, y1); break;

      /*nothing to do for these waveforms*/
      case ARB_WAVE_POS_DSINE:
      case ARB_WAVE_POS_HSINE:
      case ARB_WAVE_NEG_HSINE:
      case ARB_WAVE_NEG_DSINE:
      case ARB_WAVE_SINE:
      case ARB_WAVE_ANA_IN:
        break;

      /*no waveform? free draw*/
      default:
        bRefresh = ARB_UpdateWaveformFreeDraw(arb, graphHeight, x0, y0, x1, y1);
        break;
    }
  }

  return bRefresh;
}


/**
 * @function ARB_IsrStd
 * @brief standard ISR (put the current sample, increment current sample #id)
 * @param none
 * @return none
 */
static void ARB_IsrStd(void) {
  currentArb->currentSample += currentArb->sampleIncrement;
  if(currentArb->currentSample >= ARB_WAVEFORM_DEPTH) currentArb->currentSample = 0;
  currentArb->pOut((((uint16_t)currentArb->waveform[currentArb->currentSample]) << 8));
}


/**
 * @function ARB_IsrNoise
 * @brief noise ISR (put random)
 * @param none
 * @return none
 */
static void ARB_IsrNoise(void) {
  currentArb->pOut(P2D_Rand(0xFFFF));
}


/**
 * @function ARB_IsrAnaIn
 * @brief analog input ISR
 * @param none
 * @return none
 */
static void ARB_IsrAnaIn(void) {
  currentArb->pOut(ANA_GetInput());
}
