#include "nrf24_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#define CHIP_SELECT_PIN GPIO_Pin_7
#define CHIP_ENABLE_PIN GPIO_Pin_8

void csnLow() {
	GPIOE->BSRRH |= CHIP_SELECT_PIN; // set PE7 (CSN) low
}

void csnHigh() {
	GPIOE->BSRRL |= CHIP_SELECT_PIN; // set PE7 (CSN) high
}

void ceLow() {
  GPIOE->BSRRH |= CHIP_ENABLE_PIN; // set PE8 (CE) low
}

void ceHigh() {
  GPIOE->BSRRL |= CHIP_ENABLE_PIN; // set PE8 (CE) high
}

uint8_t getCe() {
  return GPIO_ReadInputDataBit(GPIOE, CHIP_ENABLE_PIN);
}

uint8_t spiXmitByte(uint8_t value) {
	SPI3->DR = value; // write data to be transmitted to the SPI data register
	while( !(SPI3->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	while( !(SPI3->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
	while( SPI3->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore

	return SPI3->DR; // return received data from SPI data register
}

void spiInit() {
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;

	// enable clock for used IO pins
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	// configure pins used by SPI3
	// PC10 = SCK
	// PC11 = MISO
	// PC12 = MOSI 

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	// connect SPI3 pins to SPI alternate function
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);

	// enable peripheral clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI3, ENABLE);

	/* configure SPI3 in Mode 0
	 * CPOL = 0 --> clock is low when idle
	 * CPHA = 0 --> data is sampled at the first edge
	 */
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // set to full duplex mode, seperate MOSI and MISO lines
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;     // transmit in master mode, NSS pin has to be always high
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b; // one packet of data is 8 bits wide
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;        // clock is low when idle
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;      // data sampled at first edge
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set; // set the NSS management to internal and pull internal NSS high
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // SPI frequency is APB2 frequency / 4
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;// data is transmitted MSB first
	SPI_Init(SPI1, &SPI_InitStruct);

	SPI_Cmd(SPI3, ENABLE); // enable SPI3
	_init_TIM3(); // enable Timer 3 for delay function
}

void gpioInit() {
  GPIO_InitTypeDef GPIO_InitStruct;
  // enable clock for used IO pins
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

  /* Configure the chip select and chip enable pins
   in this case we will use PE7 */
  GPIO_InitStruct.GPIO_Pin = CHIP_SELECT_PIN | CHIP_ENABLE_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStruct);

  csnHigh();
  ceLow();
}

void delayUs(uint32_t microseconds) {
  int i;
  volatile uint16_t cur = 0;
  uint32_t parts = microseconds / UINT16_MAX; // for values bigger than 16 bit ...
  uint32_t rest_us = microseconds % UINT16_MAX;

  for(i=0; i < 2 * parts; i++) {
    cur = 0;
    TIM3->CNT = 0;

    while( cur < UINT16_MAX / 2 ) {
      cur = TIM3->CNT;
    }
  }

  cur = 0;
  TIM3->CNT = 0;

  while( cur < rest_us ) {
	  cur = TIM3->CNT;
  }
}

void _init_TIM3(void) {
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM3, ENABLE);
  TIM_TimeBaseStructInit (&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period = UINT16_MAX;
  TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / 1000000) - 1; // resolution: every 1ms
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseInit (TIM3, &TIM_TimeBaseStructure);
  TIM_ITConfig (TIM3, TIM_IT_Update, ENABLE);
  TIM_Cmd (TIM3, ENABLE);
}

