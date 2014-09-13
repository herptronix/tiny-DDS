/**
 * @file wav_player.c
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

#include "wav_player.h"
#include "arb_process.h"
#include "dac.h"
#include "tmr.h"

#include "AD9834.h"
#include "spi.h"
#include "sem.h"

#ifdef ARCH_PIC32

#include "ff.h"

/**
 * private variables definition
 */
#define WAV_TIMER             ARB_TIMER
#define BUF_SIZE 4096
enum { BUFFER_1, BUFFER_2};
static int16_t buf1[BUF_SIZE];
static int16_t buf2[BUF_SIZE];
static uint8_t bufferToUse;                 /*signals to the callback which buffer shall be used*/
static uint16_t posBuf1, posBuf2;           /*buffer current position*/
static bool isBuffer1Empty, isBuffer2Empty; /*if true, the corresponding buffer shal be filled with new wav data*/
static eWavState state = WAV_STOPPED;
static FIL pFile;
static void (*pOut) (uint16_t dataIn);


/**
 * private functions prototypes
 */
//#define WavWriteDac(value)  do{if(IsSemUnlocked(spiBusy)) AD9834_SetPhase(0, 1024 + ((0x8000 + value) >> 5));} while(0)
static void Callback_1CH_16BITS(void);
static int8_t LoadBuffer(int16_t *buf);
static int8_t LoadBuffers(void);


/**
 * @function WavProcess
 * @brief WAV player task; shall be called cyclically
 * @param none
 * @return none
 */
void WavProcess(void) {

  switch(state) {

    /*WAV_STOPPED / WAV_PAUSED -> nothing to do*/
    case WAV_STOPPED:
    case WAV_PAUSED:
      break;

    /*WAV_PLAYING: check if the buffers are not empty / if the music is finished*/
    case WAV_PLAYING:
      if(LoadBuffers() < 0) {
        WavStop();
      }
      break;

    default:
      WavStop();
      break;
  }
}


/**
 * @function WavStop
 * @brief stop the WAV playback
 * @param none
 * @return none
 */
void WavStop(void) {
  if(state != WAV_STOPPED) {
    state = WAV_STOPPED;
    TmrStop(WAV_TIMER);       /*disable the callback*/
    f_close(&pFile);
    pOut(0x8000);       /*clear the DAC*/
  }
}


/**
 * @function WavPlay
 * @brief launch the WAV playback
 * @param void (*pOut) (uint16_t dataIn): function to use for playback output
 * @return none
 */
void WavPlay(void (*_pOut) (uint16_t dataIn)) {

  if(isBuffer1Empty == false && _pOut != NULL) { /*if the buffer is not empty (i.e wav file is consistant)*/
    pOut = _pOut;
    TmrLaunch(WAV_TIMER);
    state = WAV_PLAYING;
  }
  else {
    WavStop();
  }
}


/**
 * @function WavGetStatus
 * @brief return the player status
 * @param none
 * @return eWavState status
 */
eWavState WavGetStatus(void) {
  return state;
}


/**
 * @function OpenWav
 * @brief open a wav file & start playback
 * @param const char *path: file path
 * @return int8_t: -1 -> error; 0 -> ok, the file is being played
 */
int8_t OpenWav(const char *path) {

  int8_t res = -1;

  WavStop();

  /*open the file*/
  if(f_open(&pFile, "waveform.wav", FA_OPEN_EXISTING | FA_READ) == FR_OK) {

    /*TODO: check & get wav file information*/
    /*WAV files are assumed to be 16bits 1CH 44.1kHz on this project*/
    //wav_get_file_info()...

    /*load file into buffers 1 & 2*/
    isBuffer1Empty = true;
    isBuffer2Empty = true;
    LoadBuffers();

    /*configure the playback frequency & its associated callback*/
    TmrSetFrequency(WAV_TIMER, 44100);
    TmrSetCallback(WAV_TIMER, Callback_1CH_16BITS);

    /*we always start playback by using BUFFER_1*/
    bufferToUse = BUFFER_1;

    /*start playback*/
    //WavPlay();

    res = 0;
  }

  return res;
}


/**
 * @function WavPlaback_1CH_16BITS
 * @brief wav handle for 1 channel, 16bits
 * @param none
 * @return none
 */
static void Callback_1CH_16BITS(void) {

  /*use buffer 1?*/
  if(bufferToUse == BUFFER_1 && isBuffer1Empty == false) {
    pOut(0x8000 + buf1[posBuf1]);
    posBuf1++;
    if(posBuf1 >= BUF_SIZE) {
      bufferToUse = BUFFER_2;
      isBuffer1Empty = true;
    }
  }

  /*use buffer 2?*/
  else if(bufferToUse == BUFFER_2 && isBuffer2Empty == false) {
    pOut(0x8000 + buf2[posBuf2]);
    posBuf2++;
    if(posBuf2 >= BUF_SIZE) {
      bufferToUse = BUFFER_1;
      isBuffer2Empty = true;
    }
  }
}


/**
 * @function LoadBuffers
 * @brief loads buffers, from wav file to RAM (if necessary)
 * @param none
 * @return int8_t: -1: end of file / file not found, 0: ok
 */
static int8_t LoadBuffers(void) {

  int8_t res = 0;

  /*buffer 1*/
  if(isBuffer1Empty == true) {
    if(LoadBuffer(buf1) < 0) {
      res = -1;
    }
    else {
      posBuf1 = 0;
      isBuffer1Empty = false;
    }
  }

  /*buffer 2 -- copy paste of buffer 1 procedure...*/
  if(isBuffer2Empty == true) {
    if(LoadBuffer(buf2) < 0) {
      res = -1;
    }
    else {
      posBuf2 = 0;
      isBuffer2Empty = false;
    }
  }

  return res;
}


/**
 * @function LoadBuffer
 * @brief loads a given buffer with the wav file, and increase the file seeker
 * @param int16_t *buffer: buffer[BUF_SIZE] to fill
 * @return int8_t: -1: end of file & repeat = false / file not found, 0: ok
 */
static int8_t LoadBuffer(int16_t *buffer) {

  UINT readByte = 0, byteToRead = sizeof(buffer[0]) * BUF_SIZE, ii;
  int8_t res = -1;

  /*wav util content (the music, after the file header) is assumed to be 48*/
  /*TODO: implement a cleaner version...*/
  uint32_t wavContentOffset = 48;

  if(f_read(&pFile, buffer, byteToRead, &readByte) == FR_OK) {

    /*somewhere in the wav file, but not at the end of file*/
    if(readByte == byteToRead) {
      res = 0;
    }

    /*end of file, repeat*/
    else {

      f_lseek(&pFile, wavContentOffset);
      ii = readByte / sizeof(buffer[0]);
      if(f_read(&pFile, &buffer[ii], sizeof(buffer[0]) * (BUF_SIZE - ii), &readByte) == FR_OK) {
        res = 0;
      }
    }
  }

  return res;
}

#endif
