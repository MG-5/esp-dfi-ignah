#pragma once

#include "driver/i2c_types.h"
#include <cstdint>

class I2cBusAccessor
{
public:
    using DeviceAddress = uint16_t;

    virtual void beginTransaction(i2c_master_dev_handle_t deviceHandle) = 0;
    virtual void endTransaction() = 0;

    virtual i2c_master_dev_handle_t addDevice(DeviceAddress deviceAddress) = 0;

    virtual bool read(uint8_t *buffer, uint16_t length) = 0;
    virtual bool readFromRegister(uint8_t registerAdress, uint8_t *buffer, uint16_t length) = 0;
    virtual bool readByteFromRegister(uint8_t registerAdress, uint8_t &byte) = 0;
    virtual bool readWordFromRegister(uint8_t registerAdress, uint16_t &word) = 0;

    virtual bool write(const uint8_t *data, uint16_t length) = 0;
    virtual bool writeByteToRegister(uint8_t registerAdress, const uint8_t byte) = 0;
    virtual bool writeWordToRegister(uint8_t registerAdress, const uint16_t word) = 0;

    virtual ~I2cBusAccessor() = default;
};