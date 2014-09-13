/**
 * @file arb_wavedraw.h
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

#ifndef _arb_wavedraw_h_
#define _arb_wavedraw_h_

#include "usr_main.h"
#include "arb_process.h"

/**
 * @function ARB_UpdateWaveformFreeDraw
 * @brief update the samples of a undefined waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return true if need to refresh, false otherwise
 */
bool ARB_UpdateWaveformFreeDraw(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1);

/**
 * @function ARB_UpdateWaveformTriangle
 * @brief update the samples of a triangle waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return true if need to refresh, false otherwise
 */
bool ARB_UpdateWaveformTriangle(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1);

/**
 * @function ARB_UpdateWaveformPulse
 * @brief update the samples of a pulse waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return true if need to refresh, false otherwise
 */
bool ARB_UpdateWaveformPulse(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1);

/**
 * @function ARB_UpdateWaveformRC
 * @brief update the samples of a RC charge/discharge waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return true if need to refresh, false otherwise
 */
bool ARB_UpdateWaveformRC(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1);

/**
 * @function ARB_UpdateWaveformPosDSine
 * @brief update the samples of a positive full rectified sinus waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return true if need to refresh, false otherwise
 */
bool ARB_UpdateWaveformPosDSine(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1);

/**
 * @function ARB_UpdateWaveformPosHSine
 * @brief update the samples of a positive half rectified sinus waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return true if need to refresh, false otherwise
 */
bool ARB_UpdateWaveformPosHSine(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1);

/**
 * @function ARB_UpdateWaveformNegHSine
 * @brief update the samples of a negative half rectified sinus waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return true if need to refresh, false otherwise
 */
bool ARB_UpdateWaveformNegHSine(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1);

/**
 * @function ARB_UpdateWaveformNegDSine
 * @brief update the samples of a negative full rectified sinus waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return true if need to refresh, false otherwise
 */
bool ARB_UpdateWaveformNegDSine(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1);

/**
 * @function ARB_UpdateWaveformSineXX
 * @brief update the samples of a sin(x)/x waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return true if need to refresh, false otherwise
 */
bool ARB_UpdateWaveformSineXX(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1);

/**
 * @function ARB_UpdateWaveformSine
 * @brief update the samples of a sinus waveform according user input
 * @param arb_st *arb: pointer to the arbitrary waveform handler
 * @param length_t graphHeight: graph height, in pixel
 * @param coord_t x0, y0: previous touch
 * @param coord_t x1, y1: current touch
 * @return true if need to refresh, false otherwise
 */
bool ARB_UpdateWaveformSine(arb_st *arb, length_t graphHeight, coord_t x0, coord_t y0, coord_t x1, coord_t y1);

#endif
