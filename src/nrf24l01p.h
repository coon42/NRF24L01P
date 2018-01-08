#ifndef _NRF24L01P_H
#define _NRF24L01P_H

// Platform independent NRF24L01+ library

// created 09 May 2014
// modified 08 January 2017
// version: 0.88
// by coon (coon@c-base.org)

// TODO:
// - Interrupt support
// - Shockburst
// - Low energy stuff

// Every second the transmitter will be set to standby-I mode 
// for a short time to cooldown. The datasheet days this has 
// to be done every 4ms but one second seems also to work. 
// If you don't let the chip cooldown, it will stop transmitting 
// sporadicly when sending at maximum speed.
#define TX_COOLDOWN_TIME 1000

#define TRUE 1
#define FALSE 0

// custom errors (TODO: make a higher NRF layer?) 
#define NRF_OK 0;
#define NRF_DEVICE_NOT_POWERED_ON -1
#define NRF_NO_DATA_AVAILABLE -2
#define NRF_INVALID_PAYLOAD_SIZE -3

#define NRF_MAX_PAYLOAD_SIZE 32
#define PIPE_0 0
#define PIPE_1 1
#define PIPE_2 2
#define PIPE_3 3
#define PIPE_4 4
#define PIPE_5 5

// Commands
#define CMD_R_REGISTER           0x00 // Mask 
#define CMD_W_REGISTER           0x20 // Mask 
#define CMD_R_RX_PAYLOAD         0x61
#define CMD_W_TX_PAYLOAD         0xA0
#define CMD_FLUSH_TX             0xE1
#define CMD_FLUSH_RX             0xE2
#define CMD_REUSE_TX_PL          0xE3
#define CMD_R_RX_PL_WID          0x60
#define CMD_W_ACK_PAYLOAD        0xA8 // Mask
#define CMD_W_TX_PAYLOAD_NO_ACK  0xB0
#define CMD_NOP                  0xFF

// Register offsets
#define REG_CONFIG               0x00
#define REG_EN_AA                0x01
#define REG_EN_RXADDR            0x02
#define REG_SETUP_AW             0x03
#define REG_SETUP_RETR           0x04
#define REG_RF_CH                0x05
#define REG_RF_SETUP             0x06
#define REG_STATUS               0x07
#define REG_OBSERVE_TX           0x08
#define REG_RPD                  0x09
#define REG_RX_ADDR_P0           0x0A
#define REG_RX_ADDR_P1           0x0B
#define REG_RX_ADDR_P2           0x0C
#define REG_RX_ADDR_P3           0x0D
#define REG_RX_ADDR_P4           0x0E
#define REG_RX_ADDR_P5           0x0F
#define REG_TX_ADDR              0x10
#define REG_RX_PW_P0             0x11
#define REG_RX_PW_P1             0x12
#define REG_RX_PW_P2             0x13
#define REG_RX_PW_P3             0x14
#define REG_RX_PW_P4             0x15
#define REG_RX_PW_P5             0x16
#define REG_FIFO_STATUS          0x17
#define REG_DYNPD                0x1C
#define REG_FEATURE              0x1D

// Register structures
typedef struct {
  uint8_t prim_rx     : 1;
  uint8_t pwr_up      : 1;
  uint8_t crco        : 1;
  uint8_t en_crc      : 1;
  uint8_t mask_max_rt : 1;
  uint8_t mask_tx_ds  : 1;
  uint8_t mask_rx_dr  : 1;  
  uint8_t             : 1; // reserved
} RegNrf24CONFIG_t;

enum {
	CRC_MODE_OFF    = 0,
	CRC_MODE_1_BYTE = 1,
	CRC_MODE_2_BYTE = 2
};

typedef struct {
  uint8_t enaa_p0 : 1;
  uint8_t enaa_p1 : 1;
  uint8_t enaa_p2 : 1;
  uint8_t enaa_p3 : 1;
  uint8_t enaa_p4 : 1;
  uint8_t enaa_p5 : 1;
  uint8_t         : 2;
} RegNrf24EN_AA_t;

typedef struct {
  uint8_t erx_p0 : 1;
  uint8_t erx_p1 : 1;
  uint8_t erx_p2 : 1;
  uint8_t erx_p3 : 1;
  uint8_t erx_p4 : 1;
  uint8_t erx_p5 : 1;
  uint8_t        : 2;
} RegNrf24EN_RXADDR_t;

typedef struct {
  uint8_t aw : 2;
  uint8_t    : 6;
} RegNrf24SETUP_AW_t;

enum {
	AW_3BYTES  = 0x01, 
	AW_4BYTES  = 0x02,
	AW_5BYTES  = 0x03
};

typedef struct {
  uint8_t arc : 4;
  uint8_t ard : 4;
} RegNrf24SETUP_RETR_t;

enum {
  ARD_RT_DELAY_250US   = 0x00,    
  ARD_RT_DELAY_500US   = 0x01,
  ARD_RT_DELAY_750US   = 0x02,
  ARD_RT_DELAY_1000US  = 0x03,
  ARD_RT_DELAY_1250US  = 0x04,
  ARD_RT_DELAY_1500US  = 0x05,
  ARD_RT_DELAY_1750US  = 0x06,
  ARD_RT_DELAY_2000US  = 0x07,
  ARD_RT_DELAY_2250US  = 0x08,
  ARD_RT_DELAY_2500US  = 0x09,
  ARD_RT_DELAY_2750US  = 0x0A,
  ARD_RT_DELAY_3000US  = 0x0B,
  ARD_RT_DELAY_3250US  = 0x0C,
  ARD_RT_DELAY_3500US  = 0x0D,
  ARD_RT_DELAY_3750US  = 0x0E,
  ARD_RT_DELAY_4000US  = 0x0F
};

enum {
	ARC_RT_DISABLED  = 0x00,
  ARD_RT_COUNT_1   = 0x01,
  ARD_RT_COUNT_2   = 0x02,
  ARD_RT_COUNT_3   = 0x03,
  ARD_RT_COUNT_4   = 0x04,
  ARD_RT_COUNT_5   = 0x05,
  ARD_RT_COUNT_6   = 0x06,
  ARD_RT_COUNT_7   = 0x07,
  ARD_RT_COUNT_8   = 0x08,
  ARD_RT_COUNT_9   = 0x09,
  ARD_RT_COUNT_10  = 0x0A,
  ARD_RT_COUNT_11  = 0x0B,
  ARD_RT_COUNT_12  = 0x0C,
  ARD_RT_COUNT_13  = 0x0D,
  ARD_RT_COUNT_14  = 0x0E,
  ARD_RT_COUNT_15  = 0x0F
};

typedef struct {
  uint8_t rf_ch : 7;
  uint8_t       : 1;
} RegNrf24RF_CH_t;

// RF_SETUP
typedef struct {
  uint8_t lna_hcurr  : 1; // only usable on old NRF24L01 chip / obsolete on + version.
  uint8_t rf_pwr     : 2;
  uint8_t rf_dr_high : 1; // rf_dr on NRF24L01
  uint8_t pll_lock   : 1;
  uint8_t rf_dr_low  : 1; // only on NRF24L01
  uint8_t            : 1; // reserved
  uint8_t cont_wave  : 1;  // only on NRF24L01
} RegNrf24RF_SETUP_t;

// Data Rates
enum { 
	SPEED_250K = 0, 
	SPEED_1M   = 1, 
	SPEED_2M   = 2 
};

enum { 
	RF_PWR_0 = 0x00,
	RF_PWR_1 = 0x01,
	RF_PWR_2 = 0x02,
	RF_PWR_3 = 0x03 
};

typedef struct {
  uint8_t tx_full : 1;
  uint8_t rx_p_no : 3;
  uint8_t max_rt  : 1;
  uint8_t tx_ds   : 1;
  uint8_t rx_dr   : 1;
  uint8_t         : 1;
} RegNrf24STATUS_t;

enum { 
	RX_P_NO_FIFO_EMPTY = 0x07 
};

typedef struct {
  uint8_t arc_cnt  : 4;
  uint8_t plos_cnt : 4;
} RegNrf24OBSERVE_TX_t;

typedef struct {
  uint8_t rpd : 1;
  uint8_t     : 7;
} RegNrf24RPD_t;

typedef struct {
  uint8_t rx_addr_p0[5];
} RegNrf24RX_ADDR_P0_t;

typedef struct {
  uint8_t rx_addr_p1[5];
} RegNrf24RX_ADDR_P1_t;

typedef struct {
  uint8_t rx_addr_p2;
} RegNrf24RX_ADDR_P2_t;

typedef struct {
  uint8_t rx_addr_p3;
} RegNrf24RX_ADDR_P3_t;

typedef struct {
  uint8_t rx_addr_p4;
} RegNrf24RX_ADDR_P4_t;

typedef struct {
  uint8_t rx_addr_p5;
} RegNrf24RX_ADDR_P5_t;

typedef struct {
  uint8_t tx_addr[5];
} RegNrf24TX_ADDR_t;

typedef struct {
  uint8_t rx_pw_p0 : 6;
  uint8_t          : 2;
} RegNrf24RX_PW_P0_t;

typedef struct {
  uint8_t rx_pw_p1 : 6;
  uint8_t          : 2;
} RegNrf24RX_PW_P1_t;

typedef struct {
  uint8_t rx_pw_p2 : 6;
  uint8_t          : 2;
} RegNrf24RX_PW_P2_t;

typedef struct {
  uint8_t rx_pw_p3 : 6;
  uint8_t          : 2;
} RegNrf24RX_PW_P3_t;

typedef struct {
  uint8_t rx_pw_p4 : 6;
  uint8_t          : 2;
} RegNrf24RX_PW_P4_t;

typedef struct {
  uint8_t rx_pw_p5 : 6;
  uint8_t          : 2;
} RegNrf24RX_PW_P5_t;

typedef struct {
  uint8_t rx_empty : 1;
  uint8_t rx_full  : 1;
  uint8_t          : 2;
  uint8_t tx_empty : 1;
  uint8_t tx_full  : 1;
  uint8_t tx_reuse : 1;
  uint8_t          : 1;
} RegNrf24FIFO_STATUS_t;

typedef struct {
  uint8_t dpl_p0 : 1;
  uint8_t dpl_p1 : 1;
  uint8_t dpl_p2 : 1;
  uint8_t dpl_p3 : 1;
  uint8_t dpl_p4 : 1;
  uint8_t dpl_p5 : 1;
  uint8_t        : 2;
} RegNrf24DYNPD_t;

typedef struct {
  uint8_t EN_DYN_ACK : 1;
  uint8_t EN_ACK_PAY : 1;
  uint8_t EN_DPL     : 1;
  uint8_t            : 5;
} RegNrf24FEATURE_t;

void nrf24_init(); // listens to channel 81 by default
void nrf24_reset(); // TODO: reset registers to default values
void nrf24_powerUp(uint8_t enable);
void nrf24_enableCRC(uint8_t numBytes);
void nrf24_listenMode(uint8_t enable);
void nrf24_enableShockburst(uint8_t pipeId, uint8_t enable);
void nrf24_enableDataPipe(uint8_t pipeId, uint8_t enable);
void nrf24_setAddressWidth(uint8_t numBytes);
void nrf24_setRFChannel(uint8_t channel);
void nrf24_setDataRate(uint8_t dataRate);
void nrf24_setXmitPower(uint8_t powerLevel);
void nrf24_setTxAddress(uint8_t* addr);
void nrf24_setPayloadSize(uint8_t pipeId, uint8_t size);
void nrf24_setRxAddress(uint8_t pipeId, uint8_t* rxAddr); // TODO: refactor for subaddresses?
int8_t nrf24_sendPacket(void* packet, int8_t payloadSize, uint8_t listenAfterSend);

// TODO: SETUP_RETR
// RF_SETUP
// TODO: 
//   CONST_WAVE
//   PLL_LOCK

// getters
uint8_t nrf24_crcIsEnabled();
uint8_t nrf24_crcGetEncodingScheme();
uint8_t nrf24_isPoweredOn();
uint8_t nrf24_shockburstIsEnabled(uint8_t pipeId);
uint8_t nrf24_dataPipeIsEnabled(uint8_t pipeId);
uint8_t nrf24_getAddressWidths();
uint8_t nrf24_getRxAddress(uint8_t pipeId, uint8_t* rxAddr);
uint8_t nrf24_getTxAddress(uint8_t* txAddr);
uint8_t nrf24_getRFChannel();
uint8_t nrf24_getCurrentRxPipe(); // STATUS -> RX_P_NO (gets the pipeId of the data on top of the RX fifo. TODO: rename?
uint8_t nrf24_getDataRate();
uint8_t nrf24_isListening();
uint32_t nrf24_recvPacket(void* packet);
uint8_t nrf24_getPayloadSize(uint8_t pipeId);
uint8_t nrf24_getPayloadSizeRxFifoTop();
void nrf24_flushRxFifo(); // Will drop ALL elements from the RX FIFO
void nrf24_flushTxFifo(); // Will drop ALL elements from the TX FIFO
uint8_t nrf24_txFifoIsFull();
uint8_t nrf24_txFifoIsEmpty();

#endif // !_NRF24L01P_H

