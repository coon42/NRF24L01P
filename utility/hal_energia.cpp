#include "hal/energia/hal_energia.h"
#include "SPI_hotfix.h"

// SPI GPIO config
#define CHIP_ENABLE_PIN PE_0 // CE
#define CHIP_SELECT_PIN PE_1 // CSN

void csnLow() {
  digitalWrite(CHIP_SELECT_PIN, LOW);
}

void csnHigh() {
  digitalWrite(CHIP_SELECT_PIN, HIGH);
}

void ceLow() {
  digitalWrite(CHIP_ENABLE_PIN, LOW);
}

void ceHigh() {
  digitalWrite(CHIP_ENABLE_PIN, HIGH);
}

uint8_t spiXmitByte(uint8_t value) {
  return SPI.transfer(value);
}

void spiInit() {
  SPI.begin();
}

void gpioInit() {
  pinMode(CHIP_ENABLE_PIN, OUTPUT);
  pinMode(CHIP_SELECT_PIN, OUTPUT);

  csnHigh();
  ceLow();
}

uint32_t getTickMillis() {
  return millis();
}

void delayUs(uint32_t microseconds) {
  delayMicroseconds(microseconds);
}

