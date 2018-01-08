#include "nrf24_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#define CHIP_ENABLE_PIN GPIO_Pin_13
#define CHIP_SELECT_PIN GPIO_Pin_14
#define IRQ_PIN GPIO_Pin_15

// TODO: move to somewhere else!?

static void _prvInit_TIM3(void) {
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period = UINT16_MAX;
  TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / 1000000) - 1; // resolution: every 1ms
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  TIM_Cmd (TIM3, ENABLE);
}

// --

void nrf24_ceLow() {
  GPIOC->BSRRH |= CHIP_ENABLE_PIN; // set PC13 (CE) low
}

void nrf24_ceHigh() {
  GPIOC->BSRRL |= CHIP_ENABLE_PIN; // set PC13 (CE) high
}

void nrf24_csnLow() {
	GPIOC->BSRRH |= CHIP_SELECT_PIN; // set PC14 (CSN) low
}

void nrf24_csnHigh() {
	GPIOC->BSRRL |= CHIP_SELECT_PIN; // set PC14 (CSN) high
}

uint8_t nrf24_getCe() {
  return GPIO_ReadInputDataBit(GPIOC, CHIP_ENABLE_PIN);
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
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  GPIO_InitStruct.GPIO_Pin   = CHIP_SELECT_PIN | CHIP_ENABLE_PIN;
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStruct);

  nrf24_csnHigh();
  nrf24_ceLow();

	SPI_InitTypeDef SPI_InitStruct;
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
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

	SPI_InitStruct.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;        // set to full duplex mode, seperate MOSI and MISO lines
	SPI_InitStruct.SPI_Mode              = SPI_Mode_Master;                        // transmit in master mode, NSS pin has to be always high
	SPI_InitStruct.SPI_DataSize          = SPI_DataSize_8b;                        // one packet of data is 8 bits wide
	SPI_InitStruct.SPI_CPOL              = SPI_CPOL_Low;                           // clock is low when idle
	SPI_InitStruct.SPI_CPHA              = SPI_CPHA_1Edge;                         // data sampled at first edge
	SPI_InitStruct.SPI_NSS               = SPI_NSS_Soft | SPI_NSSInternalSoft_Set; // set the NSS management to internal and pull internal NSS high
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;                // SPI frequency is APB2 frequency / 4
	SPI_InitStruct.SPI_FirstBit          = SPI_FirstBit_MSB;                       // data is transmitted MSB first
	SPI_Init(SPI3, &SPI_InitStruct);

	SPI_Cmd(SPI3, ENABLE);
	_prvInit_TIM3(); // enable Timer 3 for delay function
}

void delayUs(uint32_t microseconds) {
  int i;
  volatile uint16_t cur = 0;
  uint32_t parts = microseconds / UINT16_MAX; // for values bigger than 16 bit ...
  uint32_t rest_us = microseconds % UINT16_MAX;

  for(i=0; i < 2 * parts; i++) {
    cur = 0;
    TIM3->CNT = 0;

    while(cur < UINT16_MAX / 2)
      cur = TIM3->CNT;
  }

  cur = 0;
  TIM3->CNT = 0;

  while(cur < rest_us)
	  cur = TIM3->CNT;
}

