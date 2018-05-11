#include "nrf24_hal.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#define CHIP_ENABLE_PIN GPIO_Pin_13
#define CHIP_SELECT_PIN GPIO_Pin_14
#define IRQ_PIN GPIO_Pin_15

void nrf24_ceLow() {
  // TODO: implement
}

void nrf24_ceHigh() {
  // TODO: implement
}

void nrf24_csnLow() {
  gpio_clear(GPIOA, GPIO4);
}

void nrf24_csnHigh() {
  gpio_set(GPIOA, GPIO4);
}

uint8_t nrf24_getCe() { // GPIO get?
  return 1; // TODO: implement
}

uint8_t spiXmitByte(uint8_t value) {
  uint8_t ret = (uint8_t)spi_xfer(SPI1, value);

  return ret;
}

void spiInit() {
  // TODO: not needed !?
}

void delayUs(uint32_t microseconds) {
  // TODO: implement
}

