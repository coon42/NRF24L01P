#include "hal_energia.h"
#include "Energia.h"

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
