/**
 * @file ana.h
 * @brief analog constants
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

#ifndef _ana_h_
#define _ana_h_

#define ANA_OUTPUT_MAX        500       /*output sweep from -5V to +5V*/
#define ANA_OUTPUT_MIN_VPP    60        /*minimal DDS output is 400mv peak to peak*/
#define ANA_DAC_MAX_WORD      1023      /*10bit DAC -- generic*/
#define ANA_DAC_VPP_MAX_WORD  902       /*amplitude reaches 10V at DAC=#902*/
#define ANA_DAC_VO_OFFSET     4         /*actual offset word corresponding to 0V*/
#define ANA_FREQ_MIN          10        /*minimal frequency: 1Hz*/
#define ANA_FREQ_SINE_MAX     40000000  /*maximal sinus freq: 4MHz*/
#define ANA_FREQ_TRIG_MAX     40000000  /*maximal triangle freq: 1.5MHz*/
#define MAX_SAMPLE_PER_SECOND 100000    /*maximal sample per s (through SPI)*/

#endif
