/**
 * @file arb_wavedraw.c
 * @brief arbitrary waveform draw functions
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

#include "arb_wavedraw.h"
#include <math.h>

#define PI 3.14159265f

/**
 * local functions
 */
static bool PutPoint(uint8_t *p, uint16_t pos, uint8_t val);
static bool SetWaveformPoint(arb_st *arb, length_t graphHeight, coord_t x, coord_t y);


/**
 * @function ARB_UpdateWaveformFreeDraw
 * @brief update the samples of a undefined waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return true if need to refresh, false otherwise
 */
bool ARB_UpdateWaveformFreeDraw(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1) {

  bool bRefresh = false;
  float step;
  coord_t x, y;

  if(x0 < 0 || x1 - x0 == 0) {
    bRefresh |= SetWaveformPoint(arb, graphHeight, x1, y1);
  }
  else {
    step = (float)(y1 - y0) / (x1 - x0);
    if(x0 < x1) {
      for(y = y0, x = x0; x <= x1; x++) {
        bRefresh |= SetWaveformPoint(arb, graphHeight, x, y);
        y += step;
      }
    }
    else {
      for(y = y1, x = x1; x <= x0; x++) {
        bRefresh |= SetWaveformPoint(arb, graphHeight, x, y);
        y += step;
      }
    }
  }

  return bRefresh;
}


/**
 * @function ARB_UpdateWaveformTriangle
 * @brief update the samples of a triangle waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return true if need to refresh, false otherwise
 */
bool ARB_UpdateWaveformTriangle(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1) {

  bool bRefresh = false;
  float step;
  int32_t tmpI;
  coord_t ii;
  uint16_t pos = 0;

  if(x1 > 0) {
    step = 255.0f / x1;
    for(ii = 0; ii < x1 && ii < ARB_WAVEFORM_DEPTH; ii++) {
      tmpI = ii * step;
      bRefresh |= PutPoint(&arb->waveform[0], pos++, tmpI);
    }
  }

  if(x1 < ARB_WAVEFORM_DEPTH) {
    step = 255.0f / (ARB_WAVEFORM_DEPTH - x1);
    for(ii = 0; ii < ARB_WAVEFORM_DEPTH - x1; ii++) {
      tmpI = 255 - ii * step;
      bRefresh |= PutPoint(&arb->waveform[0], pos++, tmpI);
    }
  }

  return bRefresh;
}


/**
 * @function ARB_UpdateWaveformPulse
 * @brief update the samples of a pulse waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return true if need to refresh, false otherwise
 */
bool ARB_UpdateWaveformPulse(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1) {

  bool bRefresh = false;
  coord_t ii = 0;

  if(x1 > 0) {
    for(ii = 0; ii < x1; ii++) {
      bRefresh |= PutPoint(&arb->waveform[0], ii, 255);
    }
  }

  if(ii < ARB_WAVEFORM_DEPTH) {
    for(; ii < ARB_WAVEFORM_DEPTH; ii++) {
      bRefresh |= PutPoint(&arb->waveform[0], ii, 0);
    }
  }

  return bRefresh;
}


/**
 * @function ARB_UpdateWaveformRC
 * @brief update the samples of a RC charge/discharge waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return true if need to refresh, false otherwise
 */
bool ARB_UpdateWaveformRC(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1) {

  bool bRefresh = false;
  float tau, vo, vc, vd;
  coord_t ii;

  /*compute tau; ensure that tau > 0*/
  if(x1 < 5) x1 = 5;
  tau = (float)x1 * 100 / ARB_WAVEFORM_DEPTH;

  /*compute voltage offset*/
  vo = (255.0f * exp(-((float)ARB_WAVEFORM_DEPTH / 2) / tau)) / 2;

  /*charge*/
  for(ii = 0; ii < ARB_WAVEFORM_DEPTH / 2; ii++) {
      vc = vo + 255.0f * (1.0f - exp(-(float)ii / tau));
      bRefresh |= PutPoint(&arb->waveform[0], ii, vc);
  }

  /*discharge*/
  for(; ii < ARB_WAVEFORM_DEPTH; ii++) {
      vd = 255.0f * exp(-((float)ii - ARB_WAVEFORM_DEPTH / 2) / tau) - vo;
      bRefresh |= PutPoint(&arb->waveform[0], ii, vd);
  }

  return bRefresh;
}


/**
 * @function ARB_UpdateWaveformPosDSine
 * @brief update the samples of a positive full rectified sinus waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return true if need to refresh, false otherwise
 */
bool ARB_UpdateWaveformPosDSine(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1) {

  uint16_t ii;

  for(ii = 0; ii < ARB_WAVEFORM_DEPTH; ii++) {
    PutPoint(&arb->waveform[0], ii, (uint8_t) (255.0f * sin(PI * ii / ARB_WAVEFORM_DEPTH)));
  }

  return true;
}


/**
 * @function ARB_UpdateWaveformPosHSine
 * @brief update the samples of a positive half rectified sinus waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return true if need to refresh, false otherwise
 */
bool ARB_UpdateWaveformPosHSine(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1) {

  uint16_t ii;

  for(ii = 0; ii < ARB_WAVEFORM_DEPTH / 2; ii++) {
    PutPoint(&arb->waveform[0], ii, (uint8_t) (255.0f * sin(PI * ii * 2 / ARB_WAVEFORM_DEPTH)));
  }

  for(; ii < ARB_WAVEFORM_DEPTH; ii++) {
    PutPoint(&arb->waveform[0], ii, 0);
  }

  return true;
}


/**
 * @function ARB_UpdateWaveformNegHSine
 * @brief update the samples of a negative half rectified sinus waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return true if need to refresh, false otherwise
 */
bool ARB_UpdateWaveformNegHSine(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1) {

  uint16_t ii;

  for(ii = 0; ii < ARB_WAVEFORM_DEPTH / 2; ii++) {
    PutPoint(&arb->waveform[0], ii, (uint8_t) (255.0f * (1.0f - sin(PI * ii * 2 / ARB_WAVEFORM_DEPTH))));
  }

  for(; ii < ARB_WAVEFORM_DEPTH; ii++) {
    PutPoint(&arb->waveform[0], ii, 255);
  }

  return true;
}


/**
 * @function ARB_UpdateWaveformNegDSine
 * @brief update the samples of a negative full rectified sinus waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return true if need to refresh, false otherwise
 */
bool ARB_UpdateWaveformNegDSine(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1) {

  uint16_t ii;

  for(ii = 0; ii < ARB_WAVEFORM_DEPTH; ii++) {
    PutPoint(&arb->waveform[0], ii, (uint8_t) (255.0f * (1.0f - sin(PI * ii / ARB_WAVEFORM_DEPTH))));
  }

  return true;
}


/**
 * @function ARB_UpdateWaveformSineXX
 * @brief update the samples of a sin(x)/x waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return true if need to refresh, false otherwise
 */
bool ARB_UpdateWaveformSineXX(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1) {

  coord_t ii;
  bool bRefresh = false;
  float x, sinRes, mul, res;

  if(x1 == 0) x1 = 1;
  mul = 50.0f * x1 / ARB_WAVEFORM_DEPTH;

  for(ii = 0; ii < ARB_WAVEFORM_DEPTH; ii++) {

    /*sin(x)/x for x=0*/
    if(ii == ARB_WAVEFORM_DEPTH / 2) {
      sinRes = 1.0f;
    }
    else {
      x = ((float)ii - (ARB_WAVEFORM_DEPTH / 2)) / ARB_WAVEFORM_DEPTH;
      x *= mul;
      sinRes = sin(x) / x;
    }
    res = 47.0f + 207.0f * sinRes;  /*TODO*/

    bRefresh |= PutPoint(&arb->waveform[0], ii, (uint8_t) res);
  }

  return bRefresh;
}


/**
 * @function ARB_UpdateWaveformSine
 * @brief update the samples of a sinus waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return true if need to refresh, false otherwise
 */
bool ARB_UpdateWaveformSine(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1) {

  coord_t ii;

  for(ii = 0; ii < ARB_WAVEFORM_DEPTH; ii++) {
    PutPoint(&arb->waveform[0], ii, (uint8_t) 127.0f + (126.0f * sin(2 * PI * ii / ARB_WAVEFORM_DEPTH)));
  }

  return true;
}


/**
 * @function PutPoint
 * @brief put a point at a given position
 * @param uint8_t *p: waveform buffer (address of sample #0)
 * @param uint16_t pos: point position (from 0 to ARB_WAVEFORM_DEPTH)
 * @param uint8_t val: point value
 * @return true if need to refresh, false otherwise
 */
static bool PutPoint(uint8_t *p, uint16_t pos, uint8_t val) {

  bool bRefresh = false;

  if(pos < ARB_WAVEFORM_DEPTH && p[pos] != val) {
    p[pos] = val;
    bRefresh = true;
  }

  return bRefresh;
}


/**
 * @function SetWaveformPoint
 * @brief put a point in the waveform samples list
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x, y: new point
 * @return true if need to refresh, false otherwise
 */
static bool SetWaveformPoint(arb_st *arb, length_t graphHeight, coord_t x, coord_t y) {

  float coeff = 255.0f / graphHeight;
  float tmpF = y * coeff;
  int32_t tmpI;
  bool bRefresh = false;

  tmpI = (int32_t) tmpF;
  if(tmpI < 0) tmpI = 0;
  else if(tmpI > 255) tmpI = 255;

  if(arb->waveform[x] != tmpI) {
    arb->waveform[x] = tmpI;
    bRefresh = true;
  }

  return bRefresh;
}
