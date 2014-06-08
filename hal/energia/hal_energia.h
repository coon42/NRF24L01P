#ifndef HAL_NRF24L01P_H
#define HAL_NRF24L01P_H

#include "Energia.h"

void csnLow();
void csnHigh();
void ceLow();
void ceHigh();
uint8_t getCe();
void gpioInit();
uint32_t getTickMillis();
uint8_t spiXmitByte(uint8_t value);
void spiInit();
void delayUs(uint32_t microseconds);

#endif /* HAL_H */
