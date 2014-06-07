#ifndef HAL_STM32F4_H
#define HAL_STM32F4_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_rcc.h"
#include <stdio.h>

// SPI GPIO config
#define CHIP_ENABLE_PIN GPIO_Pin_8 // CE (On rail E)
#define CHIP_SELECT_PIN GPIO_Pin_7 // CSN (On rail E)
#define UARTx USART1
#define UARTd USART2

void csnLow();
void csnHigh();
void ceLow();
void ceHigh();
uint8_t getCe();
uint8_t spiXmitByte(uint8_t value);
void spiInit();
void gpioInit();
uint32_t getTickMillis();
void delayUs(uint32_t microseconds);
void debugInterfaceInit();
void debugPrint(char* text);
void debugPrintDec(uint32_t number);
void debugPrintln(char* text);
void debugPrintHex(char* text);

void _init_TIM3(void);
void _InitializeUart(uint32_t baudrate);

#endif /* HAL_STM32F4_H */
