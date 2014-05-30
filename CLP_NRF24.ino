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
modified 30 May 2014
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
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
  Serial.begin(115200);
  radio.init(81);

  radio.enableCRC(0);
  radio.enableShockburst(0, false);
  radio.enableDataPipe(1, true);
  radio.setAddressWidth(5);
  radio.powerUp(true);
  radio.listenMode(true);

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
  
  Serial.println("TX Addresses: ");
  
  uint8_t txAddr[5];
  radio.getTxAddress(txAddr);
  Serial.print("  TX_ADDR: ");
  for(int i = 0; i < radio.getAddressWidths(); i++) {
    Serial.print("0x"); Serial.print(txAddr[i], HEX); Serial.print(" ");
  }
  
  Serial.println("Status: ");
  uint32_t rxPipe = radio.getRxPipe();
  Serial.print("  RX_FIFO: ");
  if(rxPipe != RX_P_NO_FIFO_EMPTY) {
    Serial.print("Data available on pipe: "); Serial.println(rxPipe);  
  }
  else
    Serial.println("Empty");
}

uint32_t recvBytes = 0;
uint32_t lastMicros = 0;
uint32_t passedMicros = 0;

uint32_t count = 0;
uint32_t remoteCount = 0;
bool firstFiveDone = false;
uint32_t firstFiveCtr = 0;

void loop()
{
  printFullConfig();
  
  while(true) {  
    passedMicros = micros() - lastMicros;
    
    if(radio.recvPacket(recvBuffer) != NRF_NO_DATA_AVAILABLE) {
      recvBytes += NRF_SIZE;
      remoteCount = *(uint32_t*)recvBuffer;
      
      if(!firstFiveDone) {
        firstFiveCtr++;
        if(firstFiveCtr == 5) {
          firstFiveDone = true;
          count = remoteCount;
          Serial.println("go...");
        }
        else
          continue;
      }
      
      if(count++ != remoteCount) {
        //Serial.println("ERROR!");
        count = remoteCount;
      }
    }
   
    if(passedMicros >= 1000000) {
      Serial.print("Speed: "); Serial.print((recvBytes / 1024.0) / (passedMicros / 1000000.0)); Serial.println("KB/s");
      lastMicros = micros();
      recvBytes = 0;
    }  
  }
}

