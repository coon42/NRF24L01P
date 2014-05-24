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
modified 24 May 2014
by coon
*/

#include "nrf24l01p.h"

NRF24 radio;


void setup()
{
   Serial.begin(9600);
   radio.init(81);
   
   Serial.println("NRF24L01+ Library");
   Serial.println("-----------------");
}

void printConfig() {
  radio.enableCRC(1);
  
  Serial.println("NRF24 Configuration:");
  Serial.print("RF Channel: "); Serial.println(radio.getRFChannel());
  Serial.print("CRC: "); Serial.println(radio.crcIsEnabled() ? "Enabled" : "Disabled");
  Serial.print("Encoding scheme: "); Serial.println(radio.crcGetEncodingScheme());
  Serial.print("Power Status: "); Serial.println(radio.isPoweredOn() ? "On" : "Off");
  Serial.println("Shockburst:");
  
  for(int i = 0; i < 6; i++) {
    Serial.print("  P"); Serial.print(i);
    Serial.print(": ");
    Serial.println(radio.shockburstIsEnabled(i) ? "Enabled" : "Disabled");
  }
}

void loop()
{
  printConfig();
 
  
  while(true) {
   
  }
}




