/*
NRF24L01+ example for connected launchpad

This example shows how to use this NRF24L01+ library.
Set NRF_MODE to 0 for setting the device in receiver mode
and set it to 1 for setting it to sending mode.
In sending mode the device will periodicly send a text to the
other NRF device.

You can watch the result by connecting with a serial terminal 
using the DEBUG micro USB port of your launchpad.


Pins: 
Hardware SPI:
NRF <-> Launchpad
MISO -> PE_5
MOSI -> PE_4
SCK -> PB_5

Configurable:
CE -> PE_0
CSN -> PE_1


created 09 May 2014
modified 31 May 2014
by coon (coon@c-base.org)
*/

#define NRF_MODE 0 // 0 = sender, 1 = receiver
#define NRF_PAYLOAD_SIZE 32

#include "nrf24l01p.h"

NRF24 radio;
char recvBuffer[NRF_MAX_PAYLOAD_SIZE];

void setup()
{  
  Serial.begin(115200);
  radio.init(81);
  
  uint8_t rxaddr[] = {0x01, 0x02, 0x03, 0x02, 0x01 };
  uint8_t txaddr[] = {0x01, 0x02, 0x03, 0x02, 0x01 };
  radio.setRxAddress(PIPE_0, rxaddr);
  radio.setTxAddress(txaddr);
  radio.enableCRC(1);
  radio.enableDataPipe(PIPE_0, true);
  radio.enableShockburst(PIPE_0, false);
  radio.setAddressWidth(5);
  radio.listenMode(NRF_MODE);
  radio.setDataRate(SPEED_2M);
  radio.setPayloadSize(PIPE_0, NRF_PAYLOAD_SIZE);
  radio.powerUp(true);
   
  Serial.println("NRF24L01+ Library Example");
  Serial.println("-------------------------");
  printFullConfig();
}

void printFullConfig() {  
  Serial.println("NRF24 Configuration:");
  Serial.print("Mode: "); Serial.println(radio.isListening() ? "Listening" : "Transmitting");
  Serial.print("RF Channel: "); Serial.println(radio.getRFChannel());
  Serial.print("RF Speed: "); Serial.println(radio.getDataRate() == SPEED_2M ? "2M" : radio.getDataRate() == SPEED_1M ? "1M" : "250K");
  Serial.print("CRC: "); Serial.println(radio.crcIsEnabled() ? "Enabled" : "Disabled");
  Serial.print("Encoding scheme: "); Serial.println(radio.crcGetEncodingScheme());
  Serial.print("Power Status: "); Serial.println(radio.isPoweredOn() ? "On" : "Off");
  Serial.println("Shockburst:");
  
  for(int i = 0; i < 6; i++) {
    Serial.print("  ENAA_P"); Serial.print(i);
    Serial.print(": ");
    Serial.println(radio.shockburstIsEnabled(i) ? "Enabled" : "Disabled");
  }
  
  Serial.println("RX Data Pipes:");
  for(int i = 0; i < 6; i++) {
    Serial.print("  ERX_P"); Serial.print(i); Serial.print(": ");
    Serial.println(radio.dataPipeIsEnabled(i) ? "Enabled" : "Disabled");
  }
  
  Serial.println("RX Payload sizes: ");
  for(int i = 0; i < 6; i++) {
    Serial.print("  RX_PW_P"); Serial.print(i); Serial.print(": ");
    Serial.println(radio.getPayloadSize(i));
  }
  
  
  Serial.print("Address Width: "); Serial.println(radio.getAddressWidths());
  Serial.println("RX Addresses: ");
  
  uint8_t rxAddr[5];
  for(int pipeId = 0; pipeId < 6; pipeId++) {
    radio.getRxAddress(pipeId, rxAddr);
    Serial.print("  RX_ADDR_P"); Serial.print(pipeId); Serial.print(": ");
    for(int i = 0; i < radio.getAddressWidths(); i++) {
      Serial.print("0x"); Serial.print(rxAddr[i], HEX); Serial.print(" ");
    }
    Serial.println("");
  }
  
  Serial.println("TX Address: ");
  uint8_t txAddr[5];
  radio.getTxAddress(txAddr);
  Serial.print("  TX_ADDR: ");
  for(int i = 0; i < radio.getAddressWidths(); i++) {
    Serial.print("0x"); Serial.print(txAddr[i], HEX); Serial.print(" ");
  }
  Serial.println("");
  
  bool fifoIsFull = radio.txFifoIsFull();
  Serial.print("TX_FIFO: "); Serial.println(fifoIsFull ? "Full" : "Free");
}

void loop()
{
  if(NRF_MODE == 1) { 
    // receiver mode
    int32_t recvByteCount;
    
    while(true) {
      recvByteCount = radio.recvPacket(recvBuffer);
      if(recvByteCount != NRF_NO_DATA_AVAILABLE) {
        Serial.print("Received: ");
        for(int i = 0; i < recvByteCount; i++) {
          Serial.print((char)recvBuffer[i]);
        }
        Serial.println("");
      }
    }  
  }
  else { 
    // sending mode
    while(true) {
      char* exampleText = "This text was sent over the air!";
 //     Serial.print("Sent: "); Serial.println(exampleText);
      radio.sendPacket(exampleText, NRF_PAYLOAD_SIZE, false);
      delay(1000); // wait one second 
    }
  }
}

