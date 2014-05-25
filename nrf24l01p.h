#include "Energia.h"

#define NRF24_DEBUG 1

#ifdef NRF24_DEBUG 
#define NRFDBG(functionName) Serial.println(functionName);
#else 
#define NRFDBG(functionName)
#endif

// SPI GPIO config
#define CHIP_ENABLE_PIN PE_0 // CE
#define CHIP_SELECT_PIN PE_1 // CSN


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
#define CMD_NOP                  0xFF20

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
// #define REG_ACK_PLD           N/A ???
// #define REG_TX_PLD            N/A ???
// #define REG_RX_PLD            N/A ???
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

// RF_SETUP
#define CONT_WAVE                0x40      
#define RF_DR_LOW                0x20
#define PLL_LOCK                 0x10
#define RF_DR_HIGH               0x08
enum {RF_PWR_0, RF_PWR_1, RF_PWR_2, RF_PWR_3};
enum {AW_ILLEGAL, AW_3BYTES, AW_4BYTES, AW_5BYTES};

#define RX_DR                    0x40 // 6
#define TX_DS                    0x20 // 5
#define MAX_RT                   0x10 // 4
//      RX_P_NO                       // 1..3
#define TX_FULL                  0x01 // 0                    


// TODO: add rest?



// Data Rates
enum { SPEED_250K = 0, SPEED_1M = 1, SPEED_2M = 2 };

class NRF24 {
  public:
    void init(uint8_t channel);
    void powerUp(bool enable);     // CONFIG
    void enableCRC(uint8_t numBytes);
    void listenMode(bool enable);
    void enableShockburst(byte pipeId, boolean enable); // EN_AA
    void enableDataPipe(byte pipeId, boolean enable);   // EN_RXADDR
    void setAddressWidth(uint8_t numBytes);             // SETUP_AW
    void setRFChannel(uint8_t channel);                 // RF_CH
    void setDataRate(uint8_t dataRate);                 // RF_SETUP
    void setXmitPower(uint8_t powerLevel);              // RF_SETUP
    void setRxAddress(uint8_t* addr);
    void setTxAddress(uint8_t* addr);                   // RX_ADDR_P(N)
    void setPayloadSize(uint8_t pipeId, uint8_t size);  // RX_PW_P0
    void setRxAddress(uint8_t pipeId, uint8_t* rxAddr); // RX_ADDR_P(N)
    
    // register
    uint8_t readRegister(uint8_t reg, uint8_t* dataIn);
    uint8_t readRegister(uint8_t reg, uint8_t* dataIn, uint8_t len);
    void writeRegister(uint8_t reg, uint8_t* dataOut);
    void writeRegister(uint8_t reg, uint8_t* dataOut, uint8_t len);
    
    // payload
    uint8_t readPayload(uint8_t* payload);
    
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
    bool dataIsAvailable();
    uint8_t getDataRate();
    bool isListening();
    uint8_t getPayloadSize(uint8_t pipeId);
    uint8_t getPayloadSizeRxFifoTop();
    uint32_t recvPacket(uint8_t* packet);

    void flushRxFifo();
    
private:
    void transferSync(uint8_t *dataout,uint8_t *datain,uint8_t len);
    void transmitSync(uint8_t *dataout,uint8_t len);
    
    void csnLow();
    void csnHigh();
    
    // binary helpers
    void setMask(uint8_t* var, uint8_t mask) { *var |= mask; }
    void resetMask(uint8_t* var, uint8_t mask) { *var &= ~(mask); }
    void setMaskOfRegisterIfTrue(uint8_t reg, uint8_t mask, bool set);
    
    void clearRxInterrupt();
};
