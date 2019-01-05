#include <stdint.h>
#include "nrf24l01p.h"
#include "nrf24_hal.h"

// For some reason the transmitter must not be active for more at 4ms at a time
// so it has to be set in standby I mode after 4ms beeing active for cooldown.
static uint32_t _txCooldownTimeMs;

static void readRegister(uint8_t reg, void* pData, uint8_t len) {
  int i;
  nrf24_csnLow();
  nrf24_spiXmitByte(CMD_R_REGISTER | (0x1F & reg));

  for(i = 0; i < len; i++)
    ((uint8_t*)pData)[i] = nrf24_spiXmitByte(0x00);

  nrf24_csnHigh();
}

static void readRegisterB(uint8_t reg, void* pData) {
  readRegister(reg, pData, 1);
}

static void writeRegister(uint8_t reg, void* pData, uint8_t len) {
  int i;
  nrf24_csnLow();
  uint8_t status = nrf24_spiXmitByte(CMD_W_REGISTER  | (0x1F & reg));

  for(i = 0; i < len; i++)
    nrf24_spiXmitByte(((uint8_t*)pData)[i]);

  nrf24_csnHigh();
}

static void writeRegisterB(uint8_t reg, void* pData) {
  writeRegister(reg, pData, 1);
}

static void clearRxInterrupt() {
  RegNrf24STATUS_t status;
  readRegisterB(REG_STATUS, &status);
  status.rx_dr = 1;
  writeRegisterB(REG_STATUS, &status);
}

static int8_t readPayload(uint8_t* pPayload) {
  int i;
  uint8_t payloadSize = nrf24_getPayloadSizeRxFifoTop();

  if(payloadSize > 32)
    nrf24_flushRxFifo(); // datasheet page 51 says, this is necessary

  nrf24_csnLow();
  nrf24_spiXmitByte(CMD_R_RX_PAYLOAD);

  for(i = 0; i < payloadSize; i++)
    pPayload[i] = nrf24_spiXmitByte(0x00);

  nrf24_csnHigh();
  clearRxInterrupt();

  return payloadSize;
}

static void writePayload(uint8_t* pPayload, uint8_t payloadSize) {
  int i;
  nrf24_csnLow();
  nrf24_spiXmitByte(CMD_W_TX_PAYLOAD);

  for(i = 0; i < payloadSize; i++)
    nrf24_spiXmitByte(pPayload[i]);

  nrf24_csnHigh();
}

static void checkForCooldown() {
/*
  if(!nrf24_txFifoIsEmpty())
    if(_txCooldownTimeMs == 0)
      _txCooldownTimeMs = getTickMillis(); // start the timer
    else if(getTickMillis() - _txCooldownTimeMs >= TX_COOLDOWN_TIME) {
      while(!nrf24_txFifoIsEmpty()); // wait for empty FIFO so the transmitter can go to standby I mode and gets a little cooldown.
      _txCooldownTimeMs = 0; // reset the timer
    }
*/
}

uint8_t nrf24_shockburstIsEnabled(uint8_t pipeId) {
  RegNrf24EN_AA_t enaa;
  readRegisterB(REG_EN_AA, &enaa);

  switch(pipeId) {
    case 0: return enaa.enaa_p0;
    case 1: return enaa.enaa_p1;
    case 2: return enaa.enaa_p2;
    case 3: return enaa.enaa_p3;
    case 4: return enaa.enaa_p4;
    case 5: return enaa.enaa_p5;
  }

  return 0;
}

// NRF24 API
void nrf24_enableCRC(uint8_t numBytes) {
  RegNrf24CONFIG_t config;
  readRegisterB(REG_CONFIG, &config);

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

  writeRegisterB(REG_CONFIG, &config);
}

void nrf24_powerUp(uint8_t enable) {
  RegNrf24CONFIG_t config;
  readRegisterB(REG_CONFIG, &config);
  config.pwr_up = 1;
  writeRegisterB(REG_CONFIG, &config);
}

void nrf24_listenMode(uint8_t enable) {
  // delay transition between rx and tx must be at least 130us
  // else the chip meight crash.
  nrf24_delayUs(200);

  RegNrf24CONFIG_t config;
  readRegisterB(REG_CONFIG, &config);
  config.prim_rx = enable;
  writeRegisterB(REG_CONFIG, &config);

  if(enable)
	  nrf24_ceHigh();
  else
	  nrf24_ceLow();
}

// EN_AA
void nrf24_enableShockburst(uint8_t pipeId, uint8_t enable) {
  RegNrf24EN_AA_t enaa;
  readRegisterB(REG_EN_AA, &enaa);

  switch(pipeId) {
    case 0: enaa.enaa_p0 = enable; break;
    case 1: enaa.enaa_p1 = enable; break;
    case 2: enaa.enaa_p2 = enable; break;
    case 3: enaa.enaa_p3 = enable; break;
    case 4: enaa.enaa_p4 = enable; break;
    case 5: enaa.enaa_p5 = enable; break;
  }

  writeRegisterB(REG_EN_AA, &enaa);
}

// EN_RXADDR
void nrf24_enableDataPipe(uint8_t pipeId, uint8_t enable) {
  RegNrf24EN_RXADDR_t enrxaddr;
  readRegisterB(REG_EN_RXADDR, &enrxaddr);

  switch(pipeId) {
    case 0: enrxaddr.erx_p0 = enable; break;
    case 1: enrxaddr.erx_p1 = enable; break;
    case 2: enrxaddr.erx_p2 = enable; break;
    case 3: enrxaddr.erx_p3 = enable; break;
    case 4: enrxaddr.erx_p4 = enable; break;
    case 5: enrxaddr.erx_p5 = enable; break;
  }

  writeRegisterB(REG_EN_RXADDR, &enrxaddr);
}

// SETUP_AW
void nrf24_setAddressWidth(uint8_t numBytes) {
  RegNrf24SETUP_AW_t setupaw;
  setupaw.aw = numBytes - 2;
  writeRegisterB(REG_SETUP_AW, &setupaw);
}

// RF_CH
void nrf24_setRFChannel(uint8_t channel) {
  RegNrf24RF_CH_t rfch;
  rfch.rf_ch = channel;
  writeRegisterB(REG_RF_CH, &rfch);
}

// RF_SETUP
void nrf24_setDataRate(uint8_t dataRate) {
  RegNrf24RF_SETUP_t rfSetup;
  readRegisterB(REG_RF_SETUP, &rfSetup);

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

  writeRegisterB(REG_RF_SETUP, &rfSetup);
}

void nrf24_setXmitPower(uint8_t powerLevel) {
  RegNrf24RF_SETUP_t rfSetup;
  readRegisterB(REG_RF_SETUP, &rfSetup);
  rfSetup.rf_pwr = powerLevel;
  writeRegisterB(REG_RF_SETUP, &rfSetup);
}

// TODO: refactor for subaddresses?
void nrf24_setRxAddress(uint8_t pipeId, uint8_t* pRxAddress) {
  uint8_t addressWidth = nrf24_getAddressWidths();

  switch(pipeId) {
    case 0:
      writeRegister(REG_RX_ADDR_P0, pRxAddress, addressWidth);
      break;

    case 1:
      writeRegister(REG_RX_ADDR_P1, pRxAddress, addressWidth);
      break;

    case 2:
      writeRegister(REG_RX_ADDR_P2, pRxAddress, 1);
      break;

    case 3:
      writeRegister(REG_RX_ADDR_P3, pRxAddress, 1);
      break;

    case 4:
      writeRegister(REG_RX_ADDR_P4, pRxAddress, 1);
      break;

    case 5:
      writeRegister(REG_RX_ADDR_P5, pRxAddress, 1);
      break;

    default:
      break;
  }
}

void nrf24_setTxAddress(uint8_t* pAddress) {
  writeRegister(REG_TX_ADDR, pAddress, 5);
}

void nrf24_setPayloadSize(uint8_t pipeId, uint8_t size) {
  switch(pipeId) {
    case 0: writeRegisterB(REG_RX_PW_P0, &size); break;
    case 1: writeRegisterB(REG_RX_PW_P1, &size); break;
    case 2: writeRegisterB(REG_RX_PW_P2, &size); break;
    case 3: writeRegisterB(REG_RX_PW_P3, &size); break;
    case 4: writeRegisterB(REG_RX_PW_P4, &size); break;
    case 5: writeRegisterB(REG_RX_PW_P5, &size); break;
    default: break;
  }
}

uint8_t nrf24_getRxAddress(uint8_t pipeId, uint8_t* pRxAddress) {
  uint8_t addressWidth = nrf24_getAddressWidths();

  switch(pipeId) {
    case 0:
      readRegister(REG_RX_ADDR_P0, pRxAddress, addressWidth);
      break;

    case 1:
      readRegister(REG_RX_ADDR_P1, pRxAddress, addressWidth);
      break;

    case 2:
      readRegister(REG_RX_ADDR_P1, pRxAddress, addressWidth);
      readRegisterB(REG_RX_ADDR_P2, &pRxAddress[addressWidth-1]);
      break;

    case 3:
      readRegisterB(REG_RX_ADDR_P1, pRxAddress);
      readRegisterB(REG_RX_ADDR_P3, &pRxAddress[addressWidth-1]);
      break;

    case 4:
      readRegisterB(REG_RX_ADDR_P1, pRxAddress);
      readRegisterB(REG_RX_ADDR_P4, &pRxAddress[addressWidth-1]);
      break;

    case 5:
      readRegisterB(REG_RX_ADDR_P1, pRxAddress);
      readRegisterB(REG_RX_ADDR_P5, &pRxAddress[addressWidth-1]);
      break;

    default:
      break;
  }
}

uint8_t nrf24_getTxAddress(uint8_t* pTxAddress) {
  readRegister(REG_TX_ADDR, pTxAddress, nrf24_getAddressWidths());
}

uint8_t nrf24_crcIsEnabled() {
  RegNrf24CONFIG_t config;
  readRegisterB(REG_CONFIG, &config);

  return config.en_crc;
}

uint8_t nrf24_getRFChannel() {
  RegNrf24RF_CH_t rfch;
  readRegisterB(REG_RF_CH, &rfch);

  return rfch.rf_ch;
}

uint8_t nrf24_crcGetEncodingScheme() {
  RegNrf24CONFIG_t config;
  readRegisterB(REG_CONFIG, &config);

  return config.en_crc ? config.crco ? 2 : 1 :0;
}

uint8_t nrf24_isPoweredOn() {
  RegNrf24CONFIG_t config;
  readRegisterB(REG_CONFIG, &config);

  return config.pwr_up;;
}

uint8_t nrf24_dataPipeIsEnabled(uint8_t pipeId) {
  uint8_t rxaddr;
  readRegisterB(REG_EN_RXADDR, &rxaddr);

  return rxaddr & (1 << pipeId);
}

uint8_t nrf24_getAddressWidths() {
  uint8_t setupaw;
  readRegisterB(REG_SETUP_AW, &setupaw);

  return setupaw == 3 ? 5 : setupaw == 2 ? 4 : setupaw == 1 ? 3 : 0;
}

uint8_t nrf24_getCurrentRxPipe() {
  RegNrf24STATUS_t status;
  readRegisterB(REG_STATUS, &status);

  return status.rx_p_no;
}

uint8_t nrf24_getDataRate() {
  RegNrf24RF_SETUP_t rfSetup;
  readRegisterB(REG_RF_SETUP, &rfSetup);

  if(rfSetup.rf_dr_low)
    return SPEED_250K;
  else if(rfSetup.rf_dr_high)
    return SPEED_2M;
  else
    return SPEED_1M;
}

uint8_t nrf24_isListening() {
  RegNrf24CONFIG_t config;
  readRegisterB(REG_CONFIG, &config);

  return config.prim_rx;
}

uint8_t nrf24_getPayloadSize(uint8_t pipeId) {
  uint8_t size;

  switch(pipeId) {
    case 0:
      readRegisterB(REG_RX_PW_P0, &size);
      break;

    case 1:
      readRegisterB(REG_RX_PW_P1, &size);
      break;

    case 2:
      readRegisterB(REG_RX_PW_P2, &size);
      break;

    case 3:
      readRegisterB(REG_RX_PW_P3, &size);
      break;

    case 4:
      readRegisterB(REG_RX_PW_P4, &size);
      break;

    case 5:
      readRegisterB(REG_RX_PW_P5, &size);
      break;

    default:
      break;
  }

  return size;
}

uint8_t nrf24_getPayloadSizeRxFifoTop() {
  uint8_t size;

  nrf24_csnLow();
  nrf24_spiXmitByte(CMD_R_RX_PL_WID);
  size = nrf24_spiXmitByte(0x00);
  nrf24_csnHigh();

  return size;
}

uint32_t nrf24_recvPacket(void* pPacket) {
  if(!nrf24_isPoweredOn())
    return NRF_DEVICE_NOT_POWERED_ON;

  uint8_t pipeId = nrf24_getCurrentRxPipe();
  if(pipeId == RX_P_NO_FIFO_EMPTY)
    return NRF_NO_DATA_AVAILABLE;

  return readPayload((uint8_t*)pPacket);
}

void nrf24_flushRxFifo() {
  nrf24_csnLow();
  nrf24_spiXmitByte(CMD_FLUSH_RX);
  nrf24_csnHigh();
}

void nrf24_flushTxFifo() {
  nrf24_csnLow();
  nrf24_spiXmitByte(CMD_FLUSH_TX);
  nrf24_csnHigh();
}

int8_t nrf24_sendPacket(void* pPacket, int8_t payloadSize, uint8_t listenAfterSend) {
  if(!nrf24_isPoweredOn())
    return NRF_DEVICE_NOT_POWERED_ON;

  if(payloadSize < 1 || payloadSize > 32)
    return NRF_INVALID_PAYLOAD_SIZE;

  if(nrf24_isListening())
    nrf24_listenMode(FALSE);

  while(nrf24_txFifoIsFull());
  checkForCooldown();
  writePayload((uint8_t*)pPacket, payloadSize);

  // To initially start a transmission, it is important that something in the TX FIFO
  // before pulling the chip enable pin high. If chip enable is already set when the payload is empty,
  // the transmission will NOT be initiated before ce got pulled low and high again!
  if(!nrf24_getCe()) {
    nrf24_ceHigh();
    nrf24_delayUs(10); // Delay from CE positive edge to CSN low
  }

  if(listenAfterSend)
    nrf24_listenMode(TRUE);

  return NRF_OK;
}

uint8_t nrf24_txFifoIsFull() {
  RegNrf24FIFO_STATUS_t fifostatus;
  readRegisterB(REG_FIFO_STATUS, &fifostatus);

  return fifostatus.tx_full;
}

uint8_t nrf24_txFifoIsEmpty() {
  RegNrf24FIFO_STATUS_t fifostatus;
  readRegisterB(REG_FIFO_STATUS, &fifostatus);

  return fifostatus.tx_empty;
}

uint8_t nrf24_carrierIsPresent() {
  RegNrf24RPD_t rpd;
  readRegisterB(REG_RPD, &rpd);

  return rpd.rpd;
}

void nrf24_init() {
  nrf24_spiInit();
  nrf24_csnHigh();
  nrf24_delayUs(100);

  _txCooldownTimeMs = 0;
  nrf24_flushTxFifo();
  nrf24_flushRxFifo();
  nrf24_setRFChannel(81);
  nrf24_listenMode(TRUE);
}

