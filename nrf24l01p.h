#include "Energia.h"

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
#define REG_PW_P0                0x11
#define REG_PW_P1                0x12
#define REG_PW_P2                0x13
#define REG_PW_P3                0x14
#define REG_PW_P4                0x15
#define REG_PW_P5                0x16
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

// TODO: add rest?

class NRF24 {
  public:
    NRF24(byte channel);
  
    void setRFChannel(byte channel);
    void enableShockburst(byte pipeId, boolean enable);
 
    unsigned int readRegister(byte reg);
//    void writeRegister(byte register, byte value);     
   
  uint8_t readRegister(uint8_t reg, uint8_t* dataIn, uint8_t len);
  uint8_t writeRegister(uint8_t reg, uint8_t* dataOut, uint8_t len);

 
private:
    void transferSync(uint8_t *dataout,uint8_t *datain,uint8_t len);
    void transmitSync(uint8_t *dataout,uint8_t len);
    
    void csnLow();
    void csnHigh();

};
