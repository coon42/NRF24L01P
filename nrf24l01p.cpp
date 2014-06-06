#include "SPI_hotfix.h"
#include "nrf24l01p.h"

void NRF24::csnLow() {
  digitalWrite(CHIP_SELECT_PIN, LOW);
}

void NRF24::csnHigh() {
  digitalWrite(CHIP_SELECT_PIN, HIGH);
}

void NRF24::ceLow() {
  digitalWrite(CHIP_ENABLE_PIN, LOW);
}

void NRF24::ceHigh() {
  digitalWrite(CHIP_ENABLE_PIN, HIGH);
}

void NRF24::readRegister(uint8_t reg, void* dataIn, uint8_t len) { 
  csnLow();
  SPI.transfer(CMD_R_REGISTER | (0x1F & reg));
  for(int i = 0; i < len; i++)
    ((uint8_t*)dataIn)[i] = SPI.transfer(0x00);
  csnHigh();
}

void NRF24::readRegister(uint8_t reg, void* dataIn) { 
  readRegister(reg, dataIn, 1);
}

void NRF24::writeRegister(uint8_t reg, void* dataOut, uint8_t len) {  
  csnLow();
  uint8_t status = SPI.transfer(CMD_W_REGISTER  | (0x1F & reg));
  for(int i = 0; i < len; i++)
    SPI.transfer(((uint8_t*)dataOut)[i]);
  csnHigh();
}

void NRF24::writeRegister(uint8_t reg, void* dataOut) {
  writeRegister(reg, dataOut, 1);
}

int8_t NRF24::readPayload(uint8_t* payload) {
  uint8_t payloadSize = getPayloadSizeRxFifoTop();
  
  if(payloadSize > 32) 
    flushRxFifo(); // datasheet page 51 says, this is necessary

  csnLow();
  SPI.transfer(CMD_R_RX_PAYLOAD);
  for(int i = 0; i < payloadSize; i++)
    payload[i] = SPI.transfer(0x00);  
  csnHigh();
  clearRxInterrupt();
  
  return payloadSize;
}

void NRF24::writePayload(uint8_t* payload, uint8_t payloadSize) {
  csnLow();
  SPI.transfer(CMD_W_TX_PAYLOAD);
  for(int i = 0; i < payloadSize; i++)
    SPI.transfer(payload[i]);
  csnHigh();  
}

bool NRF24::shockburstIsEnabled(uint8_t pipeId) {
  RegNrf24EN_AA_t enaa;
  readRegister(REG_EN_AA, &enaa);
  
  switch(pipeId) {
    case 0: return enaa.enaa_p0;
    case 1: return enaa.enaa_p1;
    case 2: return enaa.enaa_p2;
    case 3: return enaa.enaa_p3;
    case 4: return enaa.enaa_p4;
    case 5: return enaa.enaa_p5;
  }
}

// NRF24 API
void NRF24::enableCRC(uint8_t numBytes) {
  RegNrf24CONFIG_t config;
  readRegister(REG_CONFIG, &config);
  
  switch(numBytes) {
    case 0:
      config.en_crc = 0;
      config.crco   = 0;
      break;
    case 1:
      config.en_crc = 1;
      config.crco   = 0;
      break;
    case 2:
      config.en_crc = 1;
      config.crco   = 1;
      break;
    default:
      break;
  }
  
  writeRegister(REG_CONFIG, &config);
}

void NRF24::powerUp(bool enable) {
  RegNrf24CONFIG_t config;
  readRegister(REG_CONFIG, &config);
  config.pwr_up = 1;
  writeRegister(REG_CONFIG, &config);
}

void NRF24::listenMode(bool enable) {
  // delay transition between rx and tx must be at least 130us
  // else the chip meight crash.
  delayMicroseconds(200);
  
  RegNrf24CONFIG_t config;
  readRegister(REG_CONFIG, &config);
  config.prim_rx = enable;
  writeRegister(REG_CONFIG, &config);

  digitalWrite(CHIP_ENABLE_PIN, enable ? HIGH : LOW);
}

// EN_AA
void NRF24::enableShockburst(byte pipeId, boolean enable) {
  RegNrf24EN_AA_t enaa;
  readRegister(REG_EN_AA, &enaa);
  
  switch(pipeId) {
    case 0: enaa.enaa_p0 = enable; break;
    case 1: enaa.enaa_p1 = enable; break;
    case 2: enaa.enaa_p2 = enable; break;
    case 3: enaa.enaa_p3 = enable; break;
    case 4: enaa.enaa_p4 = enable; break;
    case 5: enaa.enaa_p5 = enable; break;
  }
  
  writeRegister(REG_EN_AA, &enaa);
}

// EN_RXADDR
void NRF24::enableDataPipe(byte pipeId, boolean enable) {
  RegNrf24EN_RXADDR_t enrxaddr;
  readRegister(REG_EN_RXADDR, &enrxaddr);
  
  switch(pipeId) {
    case 0: enrxaddr.erx_p0 = enable; break;
    case 1: enrxaddr.erx_p1 = enable; break;
    case 2: enrxaddr.erx_p2 = enable; break;
    case 3: enrxaddr.erx_p3 = enable; break;
    case 4: enrxaddr.erx_p4 = enable; break;
    case 5: enrxaddr.erx_p5 = enable; break;
  }
  
  writeRegister(REG_EN_RXADDR, &enrxaddr);
}

// SETUP_AW
void NRF24::setAddressWidth(uint8_t numBytes) {
  RegNrf24SETUP_AW_t setupaw;
  setupaw.aw = numBytes - 2;
  writeRegister(REG_SETUP_AW, &setupaw);
}

// RF_CH
void NRF24::setRFChannel(uint8_t channel) {
  RegNrf24RF_CH_t rfch;
  rfch.rf_ch = channel;
  writeRegister(REG_RF_CH, &rfch);
}

// RF_SETUP
void NRF24::setDataRate(uint8_t dataRate) {
  RegNrf24RF_SETUP_t rfSetup;
  readRegister(REG_RF_SETUP, &rfSetup);
  
  switch(dataRate) {
    case SPEED_250K:
      rfSetup.rf_dr_low  = 1;
      rfSetup.rf_dr_high = 0;
      break;
      
    case SPEED_1M:
      rfSetup.rf_dr_low  = 0;
      rfSetup.rf_dr_high = 0;
      break;
      
    case SPEED_2M:
      rfSetup.rf_dr_low  = 0;
      rfSetup.rf_dr_high = 1;   
      break;
      
    default:
      break;
  }
  
  writeRegister(REG_RF_SETUP, &rfSetup);
}

void NRF24::setXmitPower(uint8_t powerLevel) {
  RegNrf24RF_SETUP_t rfSetup;
  readRegister(REG_RF_SETUP, &rfSetup);
  rfSetup.rf_pwr = powerLevel;
  writeRegister(REG_RF_SETUP, &rfSetup);
}

// TODO: refactor for subaddresses?
void NRF24::setRxAddress(uint8_t pipeId, uint8_t* rxAddr) {
  uint8_t addressWidth = getAddressWidths();
  
  switch(pipeId) {
    case 0: 
      writeRegister(REG_RX_ADDR_P0, rxAddr, addressWidth);
      break;
    case 1:
      writeRegister(REG_RX_ADDR_P1, rxAddr, addressWidth);
      break;
    case 2:
      writeRegister(REG_RX_ADDR_P2, rxAddr, 1);
      break;
    case 3:
      writeRegister(REG_RX_ADDR_P3, rxAddr, 1);
      break;
    case 4:
      writeRegister(REG_RX_ADDR_P4, rxAddr, 1);
      break;
    case 5:
      writeRegister(REG_RX_ADDR_P5, rxAddr, 1);
      break;
    default:
      break;
  }
}


void NRF24::setTxAddress(uint8_t* addr) {
  writeRegister(REG_TX_ADDR, addr, 5);
}

void NRF24::setPayloadSize(uint8_t pipeId, uint8_t size) {
  switch(pipeId) {
    case 0: writeRegister(REG_RX_PW_P0, &size); break;
    case 1: writeRegister(REG_RX_PW_P1, &size); break;
    case 2: writeRegister(REG_RX_PW_P2, &size); break;
    case 3: writeRegister(REG_RX_PW_P3, &size); break;
    case 4: writeRegister(REG_RX_PW_P4, &size); break;
    case 5: writeRegister(REG_RX_PW_P5, &size); break;
    default: break;
  }
}

uint8_t NRF24::getRxAddress(uint8_t pipeId, uint8_t* rxAddr) {
  uint8_t addressWidth = getAddressWidths();
  
  switch(pipeId) {
    case 0: 
      readRegister(REG_RX_ADDR_P0, rxAddr, addressWidth);
      break;
    case 1:
      readRegister(REG_RX_ADDR_P1, rxAddr, addressWidth);
      break;
    case 2:
      readRegister(REG_RX_ADDR_P1, rxAddr, addressWidth);
      readRegister(REG_RX_ADDR_P2, &rxAddr[addressWidth-1]);
      break;
    case 3:
      readRegister(REG_RX_ADDR_P1, rxAddr);
      readRegister(REG_RX_ADDR_P3, &rxAddr[addressWidth-1]);
      break;
    case 4:
      readRegister(REG_RX_ADDR_P1, rxAddr);
      readRegister(REG_RX_ADDR_P4, &rxAddr[addressWidth-1]);
      break;
    case 5:
      readRegister(REG_RX_ADDR_P1, rxAddr);
      readRegister(REG_RX_ADDR_P5, &rxAddr[addressWidth-1]);
      break;
    default:
      break;
  }
}

uint8_t NRF24::getTxAddress(uint8_t* txAddr) {
  readRegister(REG_TX_ADDR, txAddr, getAddressWidths());
}

bool NRF24::crcIsEnabled() {
  RegNrf24CONFIG_t config;
  readRegister(REG_CONFIG, &config);
  return config.en_crc;
}

uint8_t NRF24::getRFChannel() {
  RegNrf24RF_CH_t rfch;
  readRegister(REG_RF_CH, &rfch);
  return rfch.rf_ch;
}

uint8_t NRF24::crcGetEncodingScheme() {
  RegNrf24CONFIG_t config;
  readRegister(REG_CONFIG, &config);
  return config.en_crc ? config.crco ? 2 : 1 :0;
}

bool NRF24::isPoweredOn() {
  RegNrf24CONFIG_t config;
  readRegister(REG_CONFIG, &config);
  return config.pwr_up;;
}

bool NRF24::dataPipeIsEnabled(uint8_t pipeId) {
  uint8_t rxaddr;
  readRegister(REG_EN_RXADDR, &rxaddr);
  return rxaddr & (1 << pipeId);
}

uint8_t NRF24::getAddressWidths() {
  uint8_t setupaw;
  readRegister(REG_SETUP_AW, &setupaw);
  return setupaw == 3 ? 5 : setupaw == 2 ? 4 : setupaw == 1 ? 3 : 0;
}

uint8_t NRF24::getCurrentRxPipe() {
  RegNrf24STATUS_t status;
  readRegister(REG_STATUS, &status);
  return status.rx_p_no;
}

uint8_t NRF24::getDataRate() {
  RegNrf24RF_SETUP_t rfSetup;
  readRegister(REG_RF_SETUP, &rfSetup);
  
  if(rfSetup.rf_dr_low)
    return SPEED_250K;
  else if(rfSetup.rf_dr_high)
    return SPEED_2M;
  else
    return SPEED_1M;
}

bool NRF24::isListening() {
  RegNrf24CONFIG_t config;
  readRegister(REG_CONFIG, &config);
  return config.prim_rx;
}

uint8_t NRF24::getPayloadSize(uint8_t pipeId) {
  uint8_t size;
  
  switch(pipeId) {
    case 0: 
      readRegister(REG_RX_PW_P0, &size);
      break;
    case 1:
      readRegister(REG_RX_PW_P1, &size);
      break;
    case 2:
      readRegister(REG_RX_PW_P2, &size);
      break;
    case 3:
      readRegister(REG_RX_PW_P3, &size);
      break;
    case 4:
      readRegister(REG_RX_PW_P4, &size);
      break;
    case 5:
      readRegister(REG_RX_PW_P5, &size);
      break;
    default:
      break;
  }
  
  return size;
}

uint8_t NRF24::getPayloadSizeRxFifoTop() {
  uint8_t size;
  
  csnLow();
  SPI.transfer(CMD_R_RX_PL_WID);
  size = SPI.transfer(0x00);
  csnHigh();
  
  return size;
}

uint32_t NRF24::recvPacket(void* packet) {
  if(!isPoweredOn())
    return NRF_DEVICE_NOT_POWERED_ON;
  
  uint8_t pipeId = getCurrentRxPipe();
  if(pipeId == RX_P_NO_FIFO_EMPTY)
    return NRF_NO_DATA_AVAILABLE;
  
  return readPayload((uint8_t*)packet);
}

void NRF24::clearRxInterrupt() {
  RegNrf24STATUS_t status;
  readRegister(REG_STATUS, &status);
  status.rx_dr = 1;
  writeRegister(REG_STATUS, &status);
}

void NRF24::flushRxFifo() {  
  csnLow();
  SPI.transfer(CMD_FLUSH_RX);
  csnHigh();
  NRFDBG("NRFDBG: RX FIFO FLUSHED!")
}

void NRF24::flushTxFifo() {
  csnLow();
  SPI.transfer(CMD_FLUSH_TX);
  csnHigh();
  NRFDBG("NRFDBG: TX FIFO FLUSHED!")
}

int8_t NRF24::sendPacket(void* packet, int8_t payloadSize, bool listenAfterSend) {
  if(!isPoweredOn())
    return NRF_DEVICE_NOT_POWERED_ON;
    
  if(payloadSize < 1 || payloadSize > 32)
    return NRF_INVALID_PAYLOAD_SIZE;
  
  if(isListening())
    listenMode(false);

  while(txFifoIsFull());
  checkForCooldown();
  writePayload((uint8_t*)packet, payloadSize);
  
  // To initially start a transmission, it is important that something in the TX FIFO
  // before pulling the chip enable pin high. If chip enable is already set when the payload is empty, 
  // the transmission will NOT be initiated before ce got pulled low and high again!   
  if(!digitalRead(CHIP_ENABLE_PIN)) {
    ceHigh();
    delayMicroseconds(10); // Delay from CE positive edge to CSN low
  }

  if(listenAfterSend)
    listenMode(true);
  
  return NRF_OK;
}

bool NRF24::txFifoIsFull() {
  RegNrf24FIFO_STATUS_t fifostatus;
  readRegister(REG_FIFO_STATUS, &fifostatus);
  return fifostatus.tx_full;
}

bool NRF24::txFifoIsEmpty() {
  RegNrf24FIFO_STATUS_t fifostatus;
  readRegister(REG_FIFO_STATUS, &fifostatus);
  return fifostatus.tx_empty;
}

void NRF24::checkForCooldown() {
  if(!txFifoIsEmpty())
    if(txTimeUs_ == 0)
      txTimeUs_ = micros(); // start the timer
    else if(micros() - txTimeUs_ >= TX_COOLDOWN_TIME) {
      while(!txFifoIsEmpty()); // wait for empty FIFO so the transmitter can go to standby I mode and gets a little cooldown.
      txTimeUs_ = 0; // reset the timer
    }
}

void NRF24::init(uint8_t channel) {
  SPI.begin();
  
  pinMode(CHIP_ENABLE_PIN, OUTPUT);
  pinMode(CHIP_SELECT_PIN, OUTPUT);
  digitalWrite(CHIP_ENABLE_PIN, LOW);
  digitalWrite(CHIP_SELECT_PIN, HIGH);

  txTimeUs_ = 0;
  flushTxFifo();   
  flushRxFifo();
  setRFChannel(channel);
  listenMode(true);
}
