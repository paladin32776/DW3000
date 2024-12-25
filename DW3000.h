// Class for lower level communication with Qorvo DW3000 chip
#ifndef DW3000_H
#define DW3000_H

#include "Arduino.h"
#include <SPI.h>

// Transaction types
#define FAST_COMMAND            0b10000001
#define SHORT_ADDRESSED_READ    0b00000000
#define SHORT_ADDRESSED_WRITE   0b10000000
#define FULL_ADDRESSED_READ     0b0100000000000000
#define FULL_ADDRESSED_WRITE    0b1100000000000000
#define MASKED_WRITE_1          0b1100000000000001
#define MASKED_WRITE_2          0b1100000000000010
#define MASKED_WRITE_4          0b1100000000000011

// Registers
#define GEN_CFG_AES     0x00
#define GEN_CFG_AES_1   0x01
#define GPIO_CTRL       0x05
#define RX_BUFFER_0     0x12
#define RX_BUFFER_1     0x13
#define TX_BUFFER       0x14

// TODO: more registers ...

// Sub-registers
#define DEV_ID          0x00
#define EUI_64          0x04
#define PANADR          0x0C
#define TX_FCTRL        0x24
#define TX_FCTRL_1      0x28
#define SYS_STATUS      0x44
#define RX_TIME         0x64
#define TX_TIME         0x74
#define GPIO_MODE       0x00  // GPIO Mode Control Register
#define GPIO_PULL_EN    0x04  // GPIO Drive Strength and Pull Control
#define GPIO_DIR        0x08  // GPIO Direction Control Register
#define GPIO_OUT        0x0C  // GPIO Data Output Register
#define GPIO_IRQE       0x10  // GPIO Interrupt Enable

// TODO: more sub-registers ...

class DW3000
{
  private:
    SPISettings spiSettings = SPISettings(1000000, MSBFIRST, SPI_MODE0);  // Define SPI settings
    int csPin = 10;
    uint8_t get_bit(uint32_t data, uint8_t bit);
    uint32_t get_bits(uint32_t data, uint8_t start_bit, uint8_t end_bit);
    uint32_t get_bits_buf(uint8_t* buffer, uint8_t start_bit, uint8_t end_bit);
    uint16_t make_header(uint16_t transaction, uint8_t _reg, uint8_t _sub_reg);
    void init();
  public:
    DW3000();
    DW3000(int csPin);
    void fast_command(uint8_t cmd);
    void short_addressed_read(uint8_t reg, uint8_t* buffer, uint16_t bytes);
    void short_addressed_write(uint8_t reg, uint8_t* buffer, uint16_t bytes);
    void full_addressed_read(uint8_t reg, uint8_t sub_reg, uint8_t* buffer, uint16_t bytes);
    void full_addressed_write(uint8_t reg, uint8_t sub_reg, uint8_t* buffer, uint16_t bytes);
    void masked_write_1(uint8_t reg, uint8_t sub_reg, uint8_t and_mask, uint8_t or_mask);
    void masked_write_2(uint8_t reg, uint8_t sub_reg, uint16_t and_mask, uint16_t or_mask);
    void masked_write_4(uint8_t reg, uint8_t sub_reg, uint32_t and_mask, uint32_t or_mask);

    void get_device_id(uint16_t* ridtag, uint8_t* model, uint8_t* ver, uint8_t* rev);
    void write_tx_buffer(uint8_t* buffer, uint16_t bytes);
    void write_txb_offset(uint16_t txb_offset);
    uint32_t read_txb_offset();

    void print8bit(uint8_t data);
    void print16bit(uint16_t data);
    void print32bit(uint32_t data);
    void printbufbits(uint8_t* buffer, uint8_t bytes);
    void printbufhex(uint8_t* buffer, uint8_t bytes);
 };

#endif // DW3000_H
