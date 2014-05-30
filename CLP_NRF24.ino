/*
NRF24L01+ example

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
modified 26 May 2014
by coon
*/

#include "nrf24l01p.h"
#include <Ethernet.h>
#include <EthernetUdp.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192,168,0,142);
unsigned int localPort = 2342;
uint8_t packetBuffer[UDP_TX_PACKET_MAX_SIZE];
EthernetUDP Udp;

NRF24 radio;
#define NRF_SIZE 32
uint8_t recvBuffer[NRF_SIZE];

void setup()
{
  pinMode(USR_SW2, INPUT_PULLUP);
  
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
  Serial.begin(115200);
  radio.init(81);

  radio.enableCRC(0);
  radio.enableShockburst(0, false);
  radio.enableDataPipe(0, true);
  radio.setAddressWidth(5);
  radio.powerUp(true);
  radio.listenMode(false);

  uint8_t rxaddr[] = {0x01, 0x02, 0x03, 0x02, 0x01 };
  uint8_t txaddr[] = {0x01, 0x02, 0x03, 0x02, 0x01 };
  radio.setRxAddress(0, rxaddr);
  radio.setTxAddress(txaddr);
  radio.setDataRate(SPEED_2M);
  radio.setPayloadSize(0, NRF_SIZE);
   
  Serial.println("NRF24L01+ Library");
  Serial.println("-----------------");
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
  
  if(radio.txFifoIsFull()) {
    Serial.println("TX FIFO FULL! STOP!!!");
    while(true);
  }
}

uint32_t count = 0;

uint32_t sentBytes = 0;
uint32_t lastMicros = 0;
uint32_t passedMicros = 0;

void loop()
{
  printFullConfig();
  
  radio.listenMode(true);
  Serial.print("CE: "); Serial.println(digitalRead(CHIP_ENABLE_PIN));
  Serial.print("Listening: "); Serial.println(radio.isListening());
  
  
  while(true) {
    int32_t size = radio.recvPacket(recvBuffer);
    
    if(size != NRF_NO_DATA_AVAILABLE)
      for(int i = 0; i < size; i++) {
        Serial.println("Data: "); Serial.println(recvBuffer[i], HEX); Serial.println(", ");
      }
  }
  
    
 /*
  while(true) {
    passedMicros = micros() - lastMicros;
    
    if(passedMicros >= 1000000) {
      Serial.print("Speed: "); Serial.print((sentBytes / 1024.0) / (passedMicros / 1000000.0)); Serial.println("KB/s");
      lastMicros = micros();
      sentBytes = 0;
    }
        
    *((uint32_t*)packetBuffer) = count++;
    while(radio.txFifoIsFull());
    radio.sendPacket(packetBuffer, NRF_SIZE, false);
    
    sentBytes += NRF_SIZE;
  }
 */ 
}

