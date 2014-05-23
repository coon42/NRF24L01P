#include "SPI.h"
#include "nrf24l01p.h"

// TODO: 
// - sleep mode (PWR_BIT setzen, CE aus etc...)
// 

void NRF24::enableShockburst(byte pipeId, boolean enable) {
//  writeRegister(REG_EN_AA, pipeId & 0x20);
}

void NRF24::setRFChannel(byte channel) {
//  SPI.transfer(REG_RF_CH | channel & 0x40);
}

/*
//Read from or write to register from the NRF24L01+:
unsigned int NRF24::readRegister(byte reg) {
 // digitalWrite(CHIP_SELECT_PIN, LOW); // take the chip select low to select the device:
  SPI.transfer(CMD_R_REGISTER | reg & 0x1F); // drop status register
  SPI.transfer(0x00);
  
  unsigned int result = SPI.transfer(CMD_R_REGISTER | reg & 0x1F); 
//  digitalWrite(CHIP_SELECT_PIN, HIGH); // take the chip select high to de-select:
  
  return result;
}
*/

/*
// Sends a write command to NF24L01+
void NRF24::writeRegister(byte reg, byte value) {  
  digitalWrite(CHIP_SELECT_PIN, LOW); // take the chip select low to select the device:
  
  SPI.transfer(CMD_W_REGISTER | (reg & 0x1F));
  SPI.transfer(value);
  
  digitalWrite(CHIP_SELECT_PIN, HIGH); // take the chip select high to de-select:
}
*/

void NRF24::csnLow() {
  digitalWrite(CHIP_SELECT_PIN, LOW);
}

void NRF24::csnHigh() {
  digitalWrite(CHIP_SELECT_PIN, HIGH);
}

uint8_t NRF24::readRegister(uint8_t reg, uint8_t* dataIn, uint8_t len) { 
  csnLow();
  
  uint8_t status = SPI.transfer(CMD_R_REGISTER | (0x1F & reg));
  for(int i = 0; i < len; i++)
    dataIn[i] = SPI.transfer(0x00);
    
  csnHigh();
  
  return status;
}

uint8_t NRF24::writeRegister(uint8_t reg, uint8_t* dataOut, uint8_t len) {
  csnLow();
      
  uint8_t status = SPI.transfer(CMD_W_REGISTER  | (0x1F & reg));
  for(int i = 0; i < len; i++)
    SPI.transfer(dataOut[i]);
  
  csnHigh();
  
  return 0; //status;
}


NRF24::NRF24(byte channel) {
  SPI.begin();
  pinMode(CHIP_ENABLE_PIN, OUTPUT);
  pinMode(CHIP_SELECT_PIN, OUTPUT);
  digitalWrite(CHIP_ENABLE_PIN, LOW);
  digitalWrite(CHIP_SELECT_PIN, HIGH);
  
  //enableShockburst(0, false);
  //writeRegister(REG_CONFIG, MASK_RX_DR | MASK_TX_DS | MASK_MAX_RT | PWR_UP);
  //writeRegister(REG_CONFIG, 0x00);
  //writeRegister(REG_CONFIG, 0x42);
  //uint8_t rv;
  //readRegister(REG_CONFIG, &rv, 1);
}
