#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "stm32f4xx_conf.h"
#include "utils.h"
#include "nrf24l01p.h"

// Private variables
volatile uint32_t time_var1;

// Private function prototypes
void Delay(volatile uint32_t nCount);
void init();

int sendUartChar(char ch) {
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
  USART_SendData(USART1, ch);

  return ch;
}

void sendUartText(const char* pText) {
  for(int i = 0; pText[i]; i++)
		sendUartChar(pText[i]);
}

void debugPrint(const char* pText) {
  sendUartText(pText);	
}

void debugPrintln(const char* pText) {
	sendUartText(pText);
  sendUartChar('\n');
}

void debugPrintDec(int n) {
  char pNum[32];
  snprintf(pNum, sizeof(pNum), "%d", n);

  sendUartText(pNum);
}

void debugPrintHex(int n) {
  char pHex[32];
  snprintf(pHex, sizeof(pHex), "0x%X", n);

  sendUartText(pHex);
}

void printFullConfig() {
  debugPrintln("NRF24 Configuration:");
  debugPrint("Mode: "); debugPrintln(nrf24_isListening() ? "Listening" : "Transmitting");
  debugPrint("RF Channel: "); debugPrintDec(nrf24_getRFChannel()); debugPrintln("");
  debugPrint("RF Speed: "); debugPrintln(nrf24_getDataRate() == SPEED_2M ? "2M" : nrf24_getDataRate() == SPEED_1M ? "1M" : "250K");
  debugPrint("CRC: "); debugPrintln(nrf24_crcIsEnabled() ? "Enabled" : "Disabled");
  debugPrint("Encoding scheme: "); debugPrintDec(nrf24_crcGetEncodingScheme()); debugPrintln("");
  debugPrint("Power Status: "); debugPrintln(nrf24_isPoweredOn() ? "On" : "Off");
  debugPrintln("Shockburst:");

  for(int i = 0; i < 6; i++) {
    debugPrint("  ENAA_P"); debugPrintDec(i);
    debugPrint(": ");
    debugPrintln(nrf24_shockburstIsEnabled(i) ? "Enabled" : "Disabled");
  }

  debugPrintln("RX Data Pipes:");
  for(int i = 0; i < 6; i++) {
    debugPrint("  ERX_P"); debugPrintDec(i); debugPrint(": ");
    debugPrintln(nrf24_dataPipeIsEnabled(i) ? "Enabled" : "Disabled");
  }

  debugPrintln("RX Payload sizes: ");
  for(int i = 0; i < 6; i++) {
    debugPrint("  RX_PW_P"); debugPrintDec(i); debugPrint(": ");
    debugPrintDec(nrf24_getPayloadSize(i)); debugPrintln("");
  }


  debugPrint("Address Width: "); debugPrintDec(nrf24_getAddressWidths()); debugPrintln("");
  debugPrintln("RX Addresses: ");

  uint8_t rxAddr[5];
  for(int pipeId = 0; pipeId < 6; pipeId++) {
    nrf24_getRxAddress(pipeId, rxAddr);
    debugPrint("  RX_ADDR_P"); debugPrintDec(pipeId); debugPrint(": ");
    for(int i = 0; i < nrf24_getAddressWidths(); i++) {
      debugPrint("0x"); debugPrintHex(rxAddr[i]); debugPrint(" ");
    }
    debugPrintln("");
  }

  debugPrintln("TX Address: ");
  uint8_t txAddr[5];
  nrf24_getTxAddress(txAddr);
  debugPrint("  TX_ADDR: ");
  for(int i = 0; i < nrf24_getAddressWidths(); i++) {
    debugPrint("0x"); debugPrintHex(txAddr[i]); debugPrint(" ");
  }
  debugPrintln("");

  uint8_t fifoIsFull = nrf24_txFifoIsFull();
  debugPrint("TX_FIFO: "); debugPrintln(fifoIsFull ? "Full" : "Free");
}

void setup_nrf24() {
  nrf24_init();

  uint8_t rxaddr[] = {0x01, 0x02, 0x03, 0x02, 0x01 };
  uint8_t txaddr[] = {0x01, 0x02, 0x03, 0x02, 0x01 };
  nrf24_setRxAddress(PIPE_0, rxaddr);
  nrf24_setTxAddress(txaddr);
  nrf24_enableCRC(CRC_MODE_OFF);
  nrf24_enableDataPipe(PIPE_0, TRUE);
  nrf24_enableShockburst(PIPE_0, FALSE);
  nrf24_setAddressWidth(5);
  nrf24_listenMode(TRUE);
  nrf24_setDataRate(SPEED_2M);
  nrf24_setPayloadSize(PIPE_0, 32);
  nrf24_powerUp(TRUE);

  debugPrintln("NRF24L01+ Library Example");
  debugPrintln("-------------------------");
  printFullConfig();
}

static void initDebugUart(uint32_t baudrate) {
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

  USART_InitStruct.USART_BaudRate = baudrate;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(USART1, &USART_InitStruct);

  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  USART_Cmd(USART1, ENABLE);
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

void init() {
  GPIO_InitTypeDef  GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  DAC_InitTypeDef  DAC_InitStructure;

  // ---------- SysTick timer -------- //
  if (SysTick_Config(SystemCoreClock / 1000)) {
    // Capture error
	while (1){};
  }

  // GPIOD Periph clock enable
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  // Configure PD12, PD13, PD14 and PD15 in output pushpull mode
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  // ---------- DAC ---------- //

  // Clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  // Configuration
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);

  // IO
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // Enable DAC Channel1
  DAC_Cmd(DAC_Channel_1, ENABLE);

  // Set DAC Channel1 DHR12L register
  DAC_SetChannel1Data(DAC_Align_12b_R, 0);

  // ------ UART ------ //
  initDebugUart(115200);

  // ------ NRF24 ------ //
  nrf24_init();
  setup_nrf24();
}

// Called from systick handler
void timing_handler() {
  if (time_var1)
    time_var1--;
}

// Delay a number of systick cycles (1ms)
void Delay(volatile uint32_t nCount) {
  time_var1 = nCount;

  while(time_var1);
}

// Dummy function to avoid compiler error
void _init() {

}

void blink() {
	GPIO_SetBits(GPIOD, GPIO_Pin_12);
	Delay(50);
  GPIO_ResetBits(GPIOD, GPIO_Pin_12);
  Delay(50);
}

int main(void) {
  init();

	int32_t recvByteCount;
  char recvBuffer[NRF_MAX_PAYLOAD_SIZE + 1];

  int i = 0;
  int ch = 0;

  while(TRUE) {
		recvByteCount = nrf24_recvPacket(recvBuffer);
    recvBuffer[recvByteCount] = '\0';
    
  	if(recvByteCount != NRF_NO_DATA_AVAILABLE) {
      blink();     
 
    	debugPrint("Received [");
      debugPrintDec(recvByteCount);
      debugPrint("]: ");

      for(int i = 0; i < recvByteCount; ++i) {
        debugPrintHex(recvBuffer[i]);
        debugPrint(" ");
      }

      debugPrintln("");   
		}

		if(i % 500000 == 0) {
			debugPrint("Set Channel to: ");
      debugPrintDec(ch);
      nrf24_setRFChannel(ch++);
      debugPrintln("");
		}
			
		i++;
	}

  return 0;
}

