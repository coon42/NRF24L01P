#ifndef HAL_ENERGIA_H
#define HAL_ENERGIA_H

#include "SPI_hotfix.h"

// SPI GPIO config (TODO: lay out in HAL)
#define CHIP_ENABLE_PIN PE_0 // CE
#define CHIP_SELECT_PIN PE_1 // CSN

void csnLow();
void csnHigh();
void ceLow();
void ceHigh();
uint8_t spiXmitByte(uint8_t value);
void spiInit();

#endif /* HAL_ENERGIA_H */
