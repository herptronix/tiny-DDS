/**
 * @file wav_player.h
 * @brief WAV player, supports only 1 channel 16bits @ 44100Hz WAV files :/
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

#ifndef _wav_player_h_
#define _wav_player_h_

#include "main.h"


typedef enum {
  WAV_PLAYING,
  WAV_PAUSED,
  WAV_STOPPED
} eWavState;

#ifdef ARCH_PIC32

/**
 * @function WavProcess
 * @brief WAV player task; shall be called cyclically
 * @param none
 * @return none
 */
void WavProcess(void);

/**
 * @function WavStop
 * @brief stop the WAV playback
 * @param none
 * @return none
 */
void WavStop(void);

/**
 * @function WavPlay
 * @brief launch the WAV playback
 * @param void (*pOut) (uint16_t dataIn): function to use for playback output
 * @return none
 */
void WavPlay(void (*_pOut) (uint16_t dataIn));

/**
 * @function WavGetStatus
 * @brief return the player status
 * @param none
 * @return eWavState status
 */
eWavState WavGetStatus(void);

/**
 * @function OpenWav
 * @brief open a wav file & start playback
 * @param const char *path: file path
 * @return int8_t: -1 -> error; 0 -> ok, the file is being played
 */
int8_t OpenWav(const char *path);

#else

#define WavPlay(a)
#define OpenWav(a) 0
#define WavProcess()
#define WavGetStatus() 0
#define WavStop()
#define WavPause()
#endif

#endif
