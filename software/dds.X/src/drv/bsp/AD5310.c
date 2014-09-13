/**
 * @file AD5310.c
 * @brief AD5310 driver (DAC)
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

#include "AD5310.h"
#include "spi.h"

#ifdef ARCH_PIC32

/**
 * struct dac_cs_st
 * CS pin descriptor
 */
typedef struct {
  volatile uint32_t *gpio;  /*output latch register*/
  volatile uint32_t *dir;   /*direction register*/
  uint16_t mask;            /*pin mask*/
} dac_cs_st;


/*list of CS pins*/
static const dac_cs_st devices[DAC_COUNT] = {
  {&LATF, &TRISF, 0x1000},  /*DAC_AMPLITUDE_CTRL*/
  {&LATD, &TRISD, 0x0001}   /*DAC_OFFSET_CRTL*/

};


/**
 * @function AD5310_Init
 * @brief init. all DAC (GPIO init & set to mid scale)
 * @param none
 * @return none
 */
void AD5310_Init(void) {

  uint16_t ii;

  for(ii = 0; ii < DAC_COUNT; ii++) {

    /*CS pin set as output pin*/
    *(devices[ii].dir) &= (devices[ii].mask ^ 0xFFFF);

    /*mid scale*/
    AD5310_Set(ii, 512);
  }
}


/**
 * @function AD5310_Set
 * @brief Set a new binary word to the selected DAC
 * @param uint8_t dev: #id of the DAC
 * @param uint16_t word_10bits: new word (only the 10LSB are used)
 * @return none
 */
void AD5310_Set(uint8_t dev, uint16_t word_10bits) {

  uint16_t data;

  if(dev < DAC_COUNT) {

    /*select the DAC*/
    SemLock(spiBusy);
    *(devices[dev].gpio) &= (devices[dev].mask ^ 0xFFFF);

    /*send the DAC word*/
    data = (word_10bits & 0x03FF) << 2;
    SPI_PutU16(data);

    /*un-select the DAC*/
    *(devices[dev].gpio) |= devices[dev].mask;
    SemUnlock(spiBusy);
  }
}

#endif
