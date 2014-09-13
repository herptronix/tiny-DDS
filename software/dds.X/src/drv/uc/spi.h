

#ifndef _spi_h_
#define _spi_h_

#include "main.h"
#include "sem.h"

extern sem_t spiBusy;

void SPI_Init(void);
void SPI_PutU16(uint16_t data);



#endif
