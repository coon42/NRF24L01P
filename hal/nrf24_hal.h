#ifndef _NRF24_H
#define _NRF24_H

#include <stdint.h>

void nrf24_ceLow();
void nrf24_ceHigh();
uint8_t nrf24_getCe();
void nrf24_csnLow();
void nrf24_csnHigh();
void nrf24_spiInit();
uint8_t nrf24_spiXmitByte(uint8_t value);

void delayUs(uint32_t microSeconds);

#endif // _NRF24_H

