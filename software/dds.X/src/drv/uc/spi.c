/**
 * @file pmp.c
 * @brief pmp driver
 * @author Duboisset Philippe
 * @version 0.1b
 * @date (yyyy-mm-dd) 2013-04-07
 *
 * Copyright (C) <2013>  Duboisset Philippe <duboisset.philippe@gmail.com>
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

#include "spi.h"
#include "gpio.h"
#include "delay.h"

sem_t spiBusy = SEM_UNLOCKED;

/**
 * @function PMP_Init
 * @brief initialize the PMP
 * @param none
 * @return none
 */
void SPI_Init(void) {
#ifdef ARCH_PIC32
  SPI3CON = 0;
  SPI3BRG = 1;
  SPI3STAT = 0;
  SPI3CONbits.MSTEN = 1;
  SPI3CONbits.CKP = 1;
  SPI3CONbits.CKE = 1;
  SPI3CONbits.MODE16 = 1;
  SPI3CONbits.ON = 1;
#endif
}

void SPI_PutU16(uint16_t data) {
#ifdef ARCH_PIC32
  SPI3BUF = data;
  while(SPI3STATbits.SPIBUSY);
#endif
}
