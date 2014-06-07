#include "stm32f4xx.h"
#include "nrf24l01p.h"

/*
NRF24L01+ example for STM32F4Discovery board.

This example shows how to use this NRF24L01+ library.
Set NRF_MODE define to NRF_MODE_RECEIVER for setting the device in receiver mode
and set it to NRF_MODE_SENDER for setting it to sending mode.
In sending mode the device will periodicly send a text to the
other NRF device.

You can watch the result by connecting with a serial terminal
using the DEBUG micro USB port of your STM32F4Discovery board.


Pins:
Hardware SPI:
NRF <-> STM32F4Discovery
SCK -> PA5
MISO -> PA6
MOSI -> PA7

Configurable:
CSN -> PE7
CE -> PE8


created 09 May 2014
modified 08 June 2014
by coon (coon@c-base.org)
*/

#include "nrf24l01p.h"
#define NRF_MODE NRF_RECEIVER
#define NRF_PAYLOAD_SIZE 32
char recvBuffer[NRF_MAX_PAYLOAD_SIZE + 1];

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


void setup()
{
  debugInterfaceInit();
  nrf24_init(81);

  uint8_t rxaddr[] = {0x01, 0x02, 0x03, 0x02, 0x01 };
  uint8_t txaddr[] = {0x01, 0x02, 0x03, 0x02, 0x01 };
  nrf24_setRxAddress(PIPE_0, rxaddr);
  nrf24_setTxAddress(txaddr);
  nrf24_enableCRC(CRC_MODE_OFF);
  nrf24_enableDataPipe(PIPE_0, TRUE);
  nrf24_enableShockburst(PIPE_0, FALSE);
  nrf24_setAddressWidth(5);
  nrf24_listenMode(NRF_MODE);
  nrf24_setDataRate(SPEED_2M);
  nrf24_setPayloadSize(PIPE_0, NRF_PAYLOAD_SIZE);
  nrf24_powerUp(TRUE);

  debugPrintln("NRF24L01+ Library Example");
  debugPrintln("-------------------------");
  printFullConfig();
}

void loop()
{
  if(NRF_MODE == NRF_RECEIVER) {
    int32_t recvByteCount;

    while(TRUE) {
      recvByteCount = nrf24_recvPacket(recvBuffer);
      recvBuffer[recvByteCount] = '\0';
      if(recvByteCount != NRF_NO_DATA_AVAILABLE) {
        debugPrint("Received: ");
        debugPrint(recvBuffer);
        debugPrintln("");
      }
    }
  }
  else { // sending mode
    while(TRUE) {
      char* exampleText = "This text was sent over the air!";
      debugPrint("Sent: "); debugPrintln(exampleText);
      nrf24_sendPacket(exampleText, NRF_PAYLOAD_SIZE, FALSE);
      delayUs(1000 * 1000); // wait one second
    }
  }
}

int main(void) {
  setup();
  while(TRUE)
    loop();
}
