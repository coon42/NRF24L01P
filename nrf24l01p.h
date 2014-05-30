#include "Energia.h"

// TODO: - low energy stuff

#define NRF24_DEBUG 1

#ifdef NRF24_DEBUG 
#define NRFDBG(functionName) Serial.println(functionName);
#else 
#define NRFDBG(functionName)
#endif

// SPI GPIO config
#define CHIP_ENABLE_PIN PE_0 // CE
#define CHIP_SELECT_PIN PE_1 // CSN


// custom errors (TODO: make a higher NRF layer?) 
#define NRF_OK 0;
#define NRF_DEVICE_NOT_POWERED_ON -1
#define NRF_NO_DATA_AVAILABLE -2
#define NRF_INVALID_PAYLOAD_SIZE -3


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

// Registers
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

// Register masks
// -----------------

// CONFIG
#define MASK_RX_DR               0x40 // 6
#define MASK_TX_DS               0x20 // 5
#define MASK_MAX_RT              0x10 // 4
#define EN_CRC                   0x08 // 3
#define CRCO                     0x04 // 2
#define PWR_UP                   0x02 // 1
#define PRIM_RX                  0x01 // 0

// EN_AA
#define ENAA_P5                  0x20
#define ENAA_P4                  0x10
#define ENAA_P3                  0x08
#define ENAA_P2                  0x04
#define ENAA_P1                  0x02
#define ENAA_P0                  0x01

// EN_RXADDR
#define ERX_P5                   0x20
#define ERX_P4                   0x10
#define ERX_P3                   0x08
#define ERX_P2                   0x04
#define ERX_P1                   0x02
#define ERX_P0                   0x01

// SETUP_AW
enum {AW_3BYTES  = 0x01, 
      AW_4BYTES  = 0x02,
      AW_5BYTES  = 0x03};

// SETUP_RETR
enum {ARD_RT_DELAY_250US   = 0x00 << 4,
      ARD_RT_DELAY_500US   = 0x01 << 4,
      ARD_RT_DELAY_750US   = 0x02 << 4,
      ARD_RT_DELAY_1000US  = 0x03 << 4,
      ARD_RT_DELAY_1250US  = 0x04 << 4,
      ARD_RT_DELAY_1500US  = 0x05 << 4,
      ARD_RT_DELAY_1750US  = 0x06 << 4,
      ARD_RT_DELAY_2000US  = 0x07 << 4,
      ARD_RT_DELAY_2250US  = 0x08 << 4,
      ARD_RT_DELAY_2500US  = 0x09 << 4,
      ARD_RT_DELAY_2750US  = 0x0A << 4,
      ARD_RT_DELAY_3000US  = 0x0B << 4,
      ARD_RT_DELAY_3250US  = 0x0C << 4,
      ARD_RT_DELAY_3500US  = 0x0D << 4,
      ARD_RT_DELAY_3750US  = 0x0E << 4,
      ARD_RT_DELAY_4000US  = 0x0F << 4};

enum {ARC_RT_DISABLED  = 0x00,
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
      ARD_RT_COUNT_15  = 0x0F};
      
// RF_CH
// Any value between 0 - 127

// RF_SETUP
#define CONT_WAVE                0x80      
#define RF_DR_LOW                0x20
#define PLL_LOCK                 0x10
#define RF_DR_HIGH               0x08
enum {RF_PWR_0  = 0x00 << 1,
      RF_PWR_1  = 0x01 << 1,
      RF_PWR_2  = 0x02 << 1,
      RF_PWR_3  = 0x03 << 1};


// STATUS
#define RX_DR                    0x40 
#define TX_DS                    0x20 
#define MAX_RT                   0x10 
enum{RX_P_NO_FIFO_EMPTY = 0x07};
#define RX_P_NO(reg_status) (reg_status & 0x0F) >> 1
#define TX_FULL                  0x01


// OBSERVE_TX
#define PLOS_CNT(reg_observeTx) reg_observeTx >> 4;
#define ARC_CNT(reg_observeTx) reg_observeTx & 0x0F;


// FIFO_STATUS
#define TX_REUSE 0x40
#define TX_FULL  0x20
#define TX_EMPTY 0x10
#define RX_FULL  0x02
#define RX_EMPTY 0x01

// DYN_PD
#define DPL_P5 0x20
#define DPL_P4 0x10
#define DPL_P3 0x08
#define DPL_P2 0x04
#define DPL_P1 0x02
#define DPL_P0 0x01

// FEATURE
#define EN_DPL     0x04
#define EN_ACK_PAY 0x02
#define EN_DYN_ACK 0x01

// Data Rates
enum { SPEED_250K = 0, SPEED_1M = 1, SPEED_2M = 2 };

class NRF24 {
  public:
    void init(uint8_t channel);
    void reset(); // TODO: reset registers to default values
    void powerUp(bool enable);     // CONFIG
    void enableCRC(uint8_t numBytes);
    void listenMode(bool enable);
    void enableShockburst(byte pipeId, boolean enable); // EN_AA
    void enableDataPipe(byte pipeId, boolean enable);   // EN_RXADDR
    void setAddressWidth(uint8_t numBytes);             // SETUP_AW
    void setRFChannel(uint8_t channel);                 // RF_CH
    void setDataRate(uint8_t dataRate);                 // RF_SETUP
    void setXmitPower(uint8_t powerLevel);              // RF_SETUP
    void setRxAddress(uint8_t* addr);  // TODO: refactor for subaddresses?
    void setTxAddress(uint8_t* addr);                   // RX_ADDR_P(N)
    void setPayloadSize(uint8_t pipeId, uint8_t size);  // RX_PW_P0
    void setRxAddress(uint8_t pipeId, uint8_t* rxAddr); // RX_ADDR_P(N)
    int8_t sendPacket(uint8_t* packet, int8_t payloadSize, bool listenAfterSend = true); // W_TX_PAYLOAD
    
    // register
    void readRegister(uint8_t reg, uint8_t* dataIn);
    void readRegister(uint8_t reg, uint8_t* dataIn, uint8_t len);
    void writeRegister(uint8_t reg, uint8_t* dataOut);
    void writeRegister(uint8_t reg, uint8_t* dataOut, uint8_t len);
    
    // TODO: SETUP_RETR

    // RF_SETUP
    // TODO: 
    //   CONST_WAVE
    //   RF_DR_LOW
    //   PLL_LOCK
    //   RF_PWR
    
    // TODO: STATUS
    // TODO: OBSERVE_TX
    // TODO: RPD
    
    // getters
    bool crcIsEnabled();  // CONFIG
    uint8_t crcGetEncodingScheme();
    bool isPoweredOn();
    bool shockburstIsEnabled(uint8_t pipeId);
    bool dataPipeIsEnabled(uint8_t pipeId);   // EN_RXADDR
    uint8_t getAddressWidths();
    uint8_t getRxAddress(uint8_t pipeId, uint8_t* rxAddr);
    uint8_t getTxAddress(uint8_t* txAddr);
    uint8_t getRFChannel(); // RF_CH
    uint8_t getRxPipe(); // STATUS -> RX_P_NO
    uint8_t getDataRate();
    bool isListening();
    uint8_t getPayloadSize(uint8_t pipeId);
    uint8_t getPayloadSizeRxFifoTop();
    uint32_t recvPacket(uint8_t* packet);

    void flushRxFifo(); // Will drop ALL elements from the RX FIFO
    void flushTxFifo(); // Will drop ALL elements from the TX FIFO
    
    bool txFifoIsFull(); // FIFO_STATUS
    
  private:
    void csnLow();
    void csnHigh();  
    void ceLow();
    void ceHigh();
    
    void clearRxInterrupt();

    // payload
    int8_t readPayload(uint8_t* payload);
    void writePayload(uint8_t* payload, uint8_t payloadSize);
    
    // binary helpers
    void setMask(uint8_t* var, uint8_t mask) { *var |= mask; }
    void resetMask(uint8_t* var, uint8_t mask) { *var &= ~(mask); }
    void setMaskOfRegisterIfTrue(uint8_t reg, uint8_t mask, bool set);
};
