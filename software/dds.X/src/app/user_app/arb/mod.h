/**
 * @file mod.h
 * @brief modulation variables
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

#ifndef _mod_h_
#define _mod_h_

#include "usr_main.h"
#include "dac.h"
#include "dds.h"

/**
 * mod_am_st
 * amplitude modulation struct
 */
typedef struct {
  int32_t frequency;
  int32_t vppMin, vppMax;
} mod_am_st;

/**
 * mod_fm_st
 * frequency modulation struct
 */
typedef struct {
  int32_t freqMin;
  int32_t freqMax;
} mod_fm_st;

/**
 * supported modulations
 */
typedef enum {
  MOD_OFF,
  MOD_AM,
  MOD_FM
} mod_type_e;

/**
 * global variables
 */
extern mod_am_st modAM;
extern mod_type_e modType;
extern mod_fm_st modFM;

#endif
