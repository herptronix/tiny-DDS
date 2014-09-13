/**
 * @file AD9834.c
 * @brief AD9834 driver (DDS chip)
 * @author Duboisset Philippe
 * @version 0.1b
 * @date (yyyy-mm-dd) 2014-04-05
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

#include "AD9834.h"
#include "spi.h"

#ifdef ARCH_PIC32

/**
 * bits composing the configuration word
 */
#define B28         0x2000    /*set to true here (frequency is 28 bits wide)*/
#define HLB         0x1000    /*ignored since B28 == true*/
#define FSEL        0x0800    /*select FREQ0 or FREQ1 registers*/
#define PSEL        0x0400    /*select PHASE0 or PHASE 1 registers*/
#define PIN_SW      0x0200    /*set to false here(functions are controlled by software, not by pins)*/
#define RESET       0x0100    /*active high; set DAC to mid scale*/
#define SLEEP1      0x0080    /*active high; suspend internal clock; DAC output remains at its present value*/
#define SLEEP12     0x0040    /*set to false here (DAC is always active)*/
#define OPBITEN     0x0020    /*active high; enable the SIGN_BIT_OUT pin*/
#define SIGN_PIB    0x0010    /*configure the SIGN_BIT_OUT*/
#define DIV2        0x0008    /*set to false, not used*/
#define MODE        0x0002    /*true = triangle output, false = sinusoidal output*/

/**
 * register addresses
 * The 2 MSB represent the address; OR'd with 14 LSB of data
 */
#define FREQ0_ADDR  0x4000
#define FREQ1_ADDR  0x8000
#define PHASE0_ADDR 0xC000
#define PHASE1_ADDR 0xE000


typedef struct {

  /*CS pin description*/
  volatile uint32_t *gpio;  /*output latch register*/
  volatile uint32_t *dir;   /*direction register*/
  uint16_t mask;            /*pin mask*/

  /*DDS configuration word*/
  uint16_t config;
} dds_st;

static dds_st devices[DDS_COUNT] = {
  {&LATF, &TRISF, 0x2000, 0x0000}
};


static void SetBit(uint8_t dev, uint16_t mask);
static void ClrBit(uint8_t dev, uint16_t mask);
static void SelectChip(uint8_t dev);
static void ReleaseChip(uint8_t dev);


/**
 * @function AD9834_Init
 * @brief initialize all listed DDS chips
 * @param none
 * @return none
 */
void AD9834_Init(void) {

  uint8_t ii;

  for(ii = 0; ii < DDS_COUNT; ii++) {

    /*CS pin set as output pin*/
    *(devices[ii].dir) &= (devices[ii].mask ^ 0xFFFF);

    /*reset internal registers*/
    AD9834_Clear(ii);
  }
}


/**
 * @function AD9834_Clear
 * @brief clear all registers of a chip and hold it in RESET
 * @param uint8_t dev: chip #id ( < DDS_COUNT)
 * @return none
 */
void AD9834_Clear(uint8_t dev) {

  if(dev < DDS_COUNT) {

    SelectChip(dev);

    /*config register*/
    devices[dev].config = 0;
    SetBit(dev, B28);
    SetBit(dev, RESET);
    SPI_PutU16(devices[dev].config);

    /*clear frequency #0*/
    SPI_PutU16(FREQ0_ADDR);
    SPI_PutU16(FREQ0_ADDR);   /*2nd write is needed*/

    /*clear frequency #1*/
    SPI_PutU16(FREQ1_ADDR);
    SPI_PutU16(FREQ1_ADDR);   /*2nd write is needed*/

    /*clear phase #0*/
    SPI_PutU16(PHASE0_ADDR);

    /*clear phase #1*/
    SPI_PutU16(PHASE1_ADDR);

    ReleaseChip(dev);
  }
}


/**
 * @function AD9834_Suspend
 * @brief stop the internal clock; DAC remains at its current value
 * @param uint8_t dev: chip #id ( < DDS_COUNT)
 * @return none
 */
void AD9834_Suspend(uint8_t dev) {

  if(dev < DDS_COUNT) {

    SelectChip(dev);

    /*config register*/
    SetBit(dev, SLEEP1);
    ClrBit(dev, RESET);
    SPI_PutU16(devices[dev].config);

    ReleaseChip(dev);
  }
}


/**
 * @function AD9834_Stop
 * @brief hold in RESET
 * @param uint8_t dev: chip #id ( < DDS_COUNT)
 * @return none
 */
void AD9834_Stop(uint8_t dev) {

  if(dev < DDS_COUNT) {

    SelectChip(dev);

    /*config register*/
    SetBit(dev, RESET);
    SPI_PutU16(devices[dev].config);

    ReleaseChip(dev);
  }
}


/**
 * @function AD9834_Resume
 * @brief restart the internal clock
 * @param uint8_t dev: chip #id ( < DDS_COUNT)
 * @return none
 */
void AD9834_Resume(uint8_t dev) {

  if(dev < DDS_COUNT) {

    SelectChip(dev);

    /*config register*/
    ClrBit(dev, RESET);
    ClrBit(dev, SLEEP1);
    SPI_PutU16(devices[dev].config);

    ReleaseChip(dev);
  }
}


/**
 * @function AD9834_SetWaveform
 * @brief select a waveform (triangle or sinus)
 * @param uint8_t dev: chip #id ( < DDS_COUNT)
 * @param waveform_e form: see waveform_e enum
 * @return none
 */
void AD9834_SetWaveform(uint8_t dev, waveform_e form) {

  if(dev < DDS_COUNT) {

    if(form == TRIANGULAR_WAVEFORM) {
      SetBit(dev, MODE);
    }
    else {
      ClrBit(dev, MODE);
    }

    SelectChip(dev);
    SPI_PutU16(devices[dev].config);
    ReleaseChip(dev);
  }
}


/**
 * @function AD9834_SetFrequency
 * @brief set the output frequency
 * @param uint8_t dev: chip #id ( < DDS_COUNT)
 * @param int32_t freq: frequency (in Hz) x 10
 * @return none
 */
void AD9834_SetFrequency(uint8_t dev, int32_t freq) {

  uint32_t freq28;
  uint16_t msb14, lsb14;

  if(dev < DDS_COUNT) {

    /*compute the frequency (coded on 28 bits, CLK@75MHz)*/
    freq28 = (uint32_t)((float)freq / 2.793967724f);

    /*split frequency in 2 14-bits words*/
    lsb14 = (uint16_t) (freq28 & 0x3FFF);
    msb14 = (uint16_t) (freq28 >> 14) & 0x3FFF;

    /*If the used FREQ register is the #1, load new freq in #0, and then use #0 (kind of double buffering) */
    if(devices[dev].config & FSEL) {
      lsb14 |= FREQ0_ADDR;
      msb14 |= FREQ0_ADDR;
      ClrBit(dev, FSEL);
    }
    /*otherwise, load in #1 an then use #1*/
    else {
      lsb14 |= FREQ1_ADDR;
      msb14 |= FREQ1_ADDR;
      SetBit(dev, FSEL);
    }

    /*send the new freq / config to the selected device*/
    SelectChip(dev);
    SPI_PutU16(lsb14);
    SPI_PutU16(msb14);
    SPI_PutU16(devices[dev].config);
    ReleaseChip(dev);
  }
}


/**
 * @function AD9834_SetPhase
 * @brief set the signal phase
 * @param uint8_t dev: chip #id ( < DDS_COUNT)
 * @param uint16_t word_12bits: phase (0=0°, 4095=360°)
 * @return none
 */
void AD9834_SetPhase(uint8_t dev, uint16_t word_12bits) {

  uint16_t tmp;

  if(dev < DDS_COUNT) {

    word_12bits &= 0x0FFF;

    /*If the used PHASE register is the #1, load new phase in #0, and then use #0 (kind of double buffering) */
    if(devices[dev].config & PSEL) {
      tmp = PHASE0_ADDR | word_12bits;
      ClrBit(dev, PSEL);
    }
    /*otherwise, load in #1 an then use #1*/
    else {
      tmp = PHASE1_ADDR | word_12bits;
      SetBit(dev, PSEL);
    }

    /*send the new phase / config to the selected device*/
    SelectChip(dev);
    SPI_PutU16(tmp);
    SPI_PutU16(devices[dev].config);
    ReleaseChip(dev);
  }
}


static void SelectChip(uint8_t dev) {
  SemLock(spiBusy);
  *(devices[dev].gpio) &= (devices[dev].mask ^ 0xFFFF);
}

static void ReleaseChip(uint8_t dev) {
  *(devices[dev].gpio) |= devices[dev].mask;
  SemUnlock(spiBusy);
}

static void SetBit(uint8_t dev, uint16_t mask) {
  devices[dev].config |= mask;
}

static void ClrBit(uint8_t dev, uint16_t mask) {
  devices[dev].config &= (0xFFFF ^ mask);
}

#endif
