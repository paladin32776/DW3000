#include "DW3000.h"

DW3000::DW3000()
{
    csPin = 10;
    init();
}

DW3000::DW3000(int _csPin)
{
    csPin = _csPin;
    init();
}   

void DW3000::init()
{
    Serial.begin(115200);
    SPI.begin();
    pinMode(csPin, OUTPUT);
    digitalWrite(csPin, HIGH);
}

uint8_t DW3000::get_bit(uint32_t data, uint8_t bit)
{
    return (data >> bit) & 1;
}

uint32_t DW3000::get_bits(uint32_t data, uint8_t start_bit, uint8_t end_bit)
{
    return (data >> start_bit) & ((1 << (end_bit - start_bit + 1)) - 1);
}

uint32_t DW3000::get_bits_buf(uint8_t* buffer, uint8_t start_bit, uint8_t end_bit)
{
    int N = end_bit / 8;
    uint32_t data = 0;
    for (int i = N; i >=0; i--)
        data = (data << 8) + buffer[i];
    return (data >> start_bit) & ((1 << (end_bit - start_bit + 1)) - 1);
}

uint16_t DW3000::make_header(uint16_t transaction, uint8_t _reg, uint8_t _sub_reg)
{
    uint8_t reg = _reg & 0b00011111;
    uint8_t sub_reg = _sub_reg & 0b01111111;
    switch (transaction)
    {
        case FAST_COMMAND:
        case SHORT_ADDRESSED_READ:
        case SHORT_ADDRESSED_WRITE:
            return transaction + (reg << 1);
        case FULL_ADDRESSED_READ:
            return transaction + (((uint16_t)reg) << 9) + (((uint16_t)sub_reg) << 2);
        case FULL_ADDRESSED_WRITE:
        case MASKED_WRITE_1:
        case MASKED_WRITE_2:
        case MASKED_WRITE_4:
            return transaction + (reg << 9) + (sub_reg << 2);
        default:
            return 0;
    }
}

void DW3000::fast_command(uint8_t cmd)
{
    uint8_t header = make_header(FAST_COMMAND, cmd, 0);
    digitalWrite(csPin, LOW);  // Select the device
    SPI.beginTransaction(spiSettings);  // Start SPI transaction
    SPI.transfer(header);
    SPI.endTransaction();  // End SPI transaction
    digitalWrite(csPin, HIGH);  // Deselect the device
}

void DW3000::short_addressed_read(uint8_t reg, uint8_t* buffer, uint16_t bytes)
{
    uint8_t header = make_header(SHORT_ADDRESSED_READ, reg, 0);
    digitalWrite(csPin, LOW);  // Select the device
    SPI.beginTransaction(spiSettings);  // Start SPI transaction
    SPI.transfer(header);
    SPI.transfer(buffer, bytes);
    SPI.endTransaction();  // End SPI transaction
    digitalWrite(csPin, HIGH);  // Deselect the device
}

void DW3000::short_addressed_write(uint8_t reg, uint8_t* buffer, uint16_t bytes)
{
    uint8_t header = make_header(SHORT_ADDRESSED_WRITE, reg, 0);
    digitalWrite(csPin, LOW);  // Select the device
    SPI.beginTransaction(spiSettings);  // Start SPI transaction
    SPI.transfer(header);
    SPI.transfer(buffer, bytes);
    SPI.endTransaction();  // End SPI transaction
    digitalWrite(csPin, HIGH);  // Deselect the device
}

void DW3000::full_addressed_read(uint8_t reg, uint8_t sub_reg, uint8_t* buffer, uint16_t bytes)
{
    uint16_t header = make_header(FULL_ADDRESSED_READ, reg, sub_reg);
    // Serial.print("reg: 0x");
    // Serial.print(reg, HEX);
    // Serial.print("  sub_reg: 0x");
    // Serial.print(sub_reg, HEX);
    // Serial.print("  header: ");
    // print16bit(header);
    // Serial.println();
    digitalWrite(csPin, LOW);  // Select the device
    SPI.beginTransaction(spiSettings);  // Start SPI transaction
    SPI.transfer(header);
    SPI.transfer(buffer, bytes);
    SPI.endTransaction();  // End SPI transaction
    digitalWrite(csPin, HIGH);  // Deselect the device
}

void DW3000::full_addressed_write(uint8_t reg, uint8_t sub_reg, uint8_t* buffer, uint16_t bytes)
{
    uint16_t header = make_header(FULL_ADDRESSED_WRITE, reg, sub_reg);
    digitalWrite(csPin, LOW);  // Select the device
    SPI.beginTransaction(spiSettings);  // Start SPI transaction
    SPI.transfer(header);
    SPI.transfer(buffer, bytes);
    SPI.endTransaction();  // End SPI transaction
    digitalWrite(csPin, HIGH);  // Deselect the device
}

void DW3000::masked_write_1(uint8_t reg, uint8_t sub_reg, uint8_t and_mask, uint8_t or_mask)
{
    uint16_t header = make_header(MASKED_WRITE_1, reg, sub_reg);
    digitalWrite(csPin, LOW);  // Select the device
    SPI.beginTransaction(spiSettings);  // Start SPI transaction
    SPI.transfer(header);
    SPI.transfer(and_mask);
    SPI.transfer(or_mask);
    SPI.endTransaction();  // End SPI transaction
    digitalWrite(csPin, HIGH);  // Deselect the device
}

void DW3000::masked_write_2(uint8_t reg, uint8_t sub_reg, uint16_t and_mask, uint16_t or_mask)
{
    uint16_t header = make_header(MASKED_WRITE_2, reg, sub_reg);
    digitalWrite(csPin, LOW);  // Select the device
    SPI.beginTransaction(spiSettings);  // Start SPI transaction
    SPI.transfer(header);
    SPI.transfer(and_mask);
    SPI.transfer(or_mask);
    SPI.endTransaction();  // End SPI transaction
    digitalWrite(csPin, HIGH);  // Deselect the device
}

void DW3000::masked_write_4(uint8_t reg, uint8_t sub_reg, uint32_t and_mask, uint32_t or_mask)
{
    uint16_t header = make_header(MASKED_WRITE_4, reg, sub_reg);
    digitalWrite(csPin, LOW);  // Select the device
    SPI.beginTransaction(spiSettings);  // Start SPI transaction
    SPI.transfer(header);
    SPI.transfer(and_mask);
    SPI.transfer(or_mask);
    SPI.endTransaction();  // End SPI transaction
    digitalWrite(csPin, HIGH);  // Deselect the device
}

void DW3000::get_device_id(uint16_t* ridtag, uint8_t* model, uint8_t* ver, uint8_t* rev)
{
    uint8_t buffer[4];
    full_addressed_read(GEN_CFG_AES, DEV_ID, buffer, 4);
    *ridtag = get_bits_buf(buffer, 16, 31);
    *model = get_bits_buf(buffer, 8, 15);
    *ver = get_bits_buf(buffer, 4, 7);
    *rev = get_bits_buf(buffer, 0, 3);
}

void DW3000::write_tx_buffer(uint8_t* buffer, uint16_t bytes)
{
    if (bytes > 1024)
        bytes = 1024;
    short_addressed_write(TX_BUFFER, buffer, bytes);
}

void DW3000::write_txb_offset(uint16_t txb_offset)
{
    uint16_t or_mask = txb_offset & 0x03FF;
    uint16_t and_mask = or_mask | 0xFC00;
    masked_write_2(GEN_CFG_AES, TX_FCTRL, and_mask, or_mask);
    Serial.print("txb_offset: 0b      ");
    Serial.println(txb_offset, BIN);
    Serial.print("   or_mask: 0b      ");
    Serial.println(or_mask, BIN);
    Serial.print("  and_mask: 0b");
    Serial.println(and_mask, BIN);    
}  

uint32_t DW3000::read_txb_offset()
{
    uint8_t buffer[4];
    full_addressed_read(GEN_CFG_AES, TX_FCTRL, buffer, 4);
    return (buffer[3]<<24) + (buffer[2]<<16) + (buffer[1]<<8) + buffer[0];
}

void DW3000::print8bit(uint8_t data)
{
    for (int i = 7; i >= 0; i--)
        Serial.print(get_bit(data, i));
}

void DW3000::print16bit(uint16_t data)
{
    for (int i = 15; i >= 0; i--)
        Serial.print(get_bit(data, i));
}

void DW3000::print32bit(uint32_t data)
{
    for (int i = 31; i >= 0; i--)
        Serial.print(get_bit(data, i));
}

void DW3000::printbufbits(uint8_t* buffer, uint8_t bytes)
{
    for (int i = bytes - 1; i >= 0; i--)
    {
        print8bit(buffer[i]);
        Serial.print(" ");
    }
}

void DW3000::printbufhex(uint8_t* buffer, uint8_t bytes)
{
    for (int i = bytes - 1; i >= 0; i--)
    {
        if (buffer[i] < 0x10)
            Serial.print("0");
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
    }
}