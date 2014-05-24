#include "SPI.h"
#include "nrf24l01p.h"

// TODO: 
// - sleep mode (PWR_BIT setzen, CE aus etc...)
// 



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

uint8_t NRF24::readRegister(uint8_t reg, uint8_t* dataIn) { 
  readRegister(reg, dataIn, 1);
}

uint8_t NRF24::writeRegister(uint8_t reg, uint8_t* dataOut, uint8_t len) {
  NRFDBG("NRF24::writeRegister(uint8_t reg, uint8_t* dataOut, uint8_t len)");
  
  csnLow();
      
  uint8_t status = SPI.transfer(CMD_W_REGISTER  | (0x1F & reg));
  for(int i = 0; i < len; i++)
    SPI.transfer(dataOut[i]);
  
  csnHigh();
  
  return 0; //status;
}

uint8_t NRF24::writeRegister(uint8_t reg, uint8_t* dataOut) {
  NRFDBG("NRF24::writeRegister(uint8_t reg, uint8_t* dataOut)");
  
  writeRegister(reg, dataOut, 1);
}

void NRF24::setMaskOfRegisterIfTrue(uint8_t reg, uint8_t mask, bool set) {
  uint8_t regContent;
  readRegister(reg, &regContent);
  
  if(set)
    setMask(&regContent, mask);
  else
    resetMask(&regContent, mask);
    
  writeRegister(reg, &regContent);
}

bool NRF24::shockburstIsEnabled(uint8_t pipeId) {
  uint8_t enaa;
  readRegister(REG_EN_AA, &enaa);
  return enaa & (1 << pipeId);
}

// NRF24 API

void NRF24::enableCRC(uint8_t numBytes) {
  uint8_t config;
  readRegister(REG_CONFIG, &config);
        
  switch(numBytes) {
    case 0:
      resetMask(&config, EN_CRC);
      resetMask(&config, CRCO);
      break;
    case 1:
      setMask(&config, EN_CRC);
      resetMask(&config, CRCO);
      break;
    case 2:
      setMask(&config, EN_CRC);
      setMask(&config, CRCO);
      break;
    default:
      break;
  }
    
  writeRegister(REG_CONFIG, &config);
}

void NRF24::powerUp(bool enable) {
  setMaskOfRegisterIfTrue(REG_CONFIG, PWR_UP, enable);
}

void NRF24::listenMode(bool enable) {
  setMaskOfRegisterIfTrue(REG_CONFIG, PRIM_RX, enable);
  digitalWrite(CHIP_ENABLE_PIN, enable ? HIGH : LOW);
}


// EN_AA

void NRF24::enableShockburst(byte pipeId, boolean enable) {
  setMaskOfRegisterIfTrue(REG_EN_AA, 1 << pipeId, enable);
}

// EN_RXADDR
void NRF24::enableDataPipe(byte pipeId, boolean enable) {
  setMaskOfRegisterIfTrue(REG_EN_RXADDR, 1 << pipeId, enable);
}

// RF_CH
void NRF24::setRFChannel(uint8_t channel) {
  writeRegister(REG_RF_CH, &channel);
}

// RF_SETUP
void NRF24::setDataRate(uint8_t dataRate) {
  uint8_t rfSetup;
  readRegister(REG_RF_SETUP, &rfSetup);
  
  switch(dataRate) {
    case SPEED_250K:
      resetMask(&rfSetup, RF_DR_HIGH);
      setMask(&rfSetup, RF_DR_LOW);
      break;
      
    case SPEED_1M:
      resetMask(&rfSetup, RF_DR_HIGH);
      resetMask(&rfSetup, RF_DR_LOW);
      break;
      
    case SPEED_2M:
      setMask(&rfSetup, RF_DR_HIGH);
      resetMask(&rfSetup, RF_DR_LOW);
      break;
      
    default:
      break;
  }
  
  writeRegister(REG_RF_SETUP, &rfSetup);
}

void NRF24::setXmitPower(uint8_t powerLevel) {
  uint8_t rfSetup;
  readRegister(REG_RF_SETUP, &rfSetup);
  resetMask(&rfSetup, 3 << 1);
  
  switch(powerLevel) {  
    case RF_PWR_0:
      break;
    case RF_PWR_1:
      setMask(&rfSetup, 1 << 1);
      break;
    case RF_PWR_2:
      setMask(&rfSetup, 2 << 1);
      break;
    case RF_PWR_3:
      setMask(&rfSetup, 3 << 1);
      break;
    default:
      break;
  }
  
  writeRegister(REG_RF_SETUP, &rfSetup);
}

// TODO: support all pipes. only pipe 0 is supported yet.
void NRF24::setRxAddr(uint8_t* addr) {
  writeRegister(REG_RX_ADDR_P1, addr, 5);
}

// TODO: support all pipes. only pipe 0 is supported yet.
void NRF24::setTxAddr(uint8_t* addr) {
  writeRegister(REG_TX_ADDR, addr, 5);
}

void NRF24::setPayloadSize(uint8_t size) {
  writeRegister(REG_RX_PW_P0, &size);
}

bool NRF24::crcIsEnabled() {
  uint8_t config;
  readRegister(REG_CONFIG, &config);
  return config & EN_CRC;
}

uint8_t NRF24::getRFChannel() {
  uint8_t channel;
  readRegister(REG_RF_CH, &channel);
  return channel;
}

uint8_t NRF24::crcGetEncodingScheme() {
  uint8_t config;
  readRegister(REG_CONFIG, &config);
  return config & EN_CRC ? config & CRCO ? 2 : 1 :0;
}

bool NRF24::isPoweredOn() {
  uint8_t config;
  readRegister(REG_CONFIG, &config);
  return config & PWR_UP;
}

void NRF24::init(uint8_t channel) {
  SPI.begin();
  pinMode(CHIP_ENABLE_PIN, OUTPUT);
  pinMode(CHIP_SELECT_PIN, OUTPUT);
  digitalWrite(CHIP_ENABLE_PIN, LOW);
  digitalWrite(CHIP_SELECT_PIN, HIGH);
  
  setRFChannel(channel);
}
