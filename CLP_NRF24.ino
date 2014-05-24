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

void setup()
{
   Serial.begin(9600);
   Serial.println("NRF24L01+ Library\n");
   Serial.println("-----------------\n");
}

void loop()
{
  NRF24 radio(81);
  uint8_t r[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  uint8_t rv[] = {0x23};
  radio.writeRegister(REG_CONFIG, rv, 1);
  radio.readRegister(REG_CONFIG, r, 8);
  
  Serial.print("CONFIG: ");
  Serial.print(r[0]);
 
  
  while(true) {
   
  }
}




