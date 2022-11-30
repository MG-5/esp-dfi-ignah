#pragma once

#include "I2cBusAccessor.hpp"
#include "helpers/freertos.hpp"
#include "wrappers/BinarySemaphore.hpp"
#include "wrappers/Mutex.hpp"

#include "driver/i2c.h"
#include "esp_log.h"

#include <string>

class EspI2cBusAccessor : public I2cBusAccessor
{
public:
    explicit EspI2cBusAccessor(i2c_port_t i2cNumber) : i2cNumber(i2cNumber)
    {
        printTag = "[I2cBusaccessor port: ";
        printTag += std::to_string(i2cNumber);
        printTag += ")]";
    }

    //--------------------------------------------------------------------------------------------------
    /// @param newAddress 7-bit device address
    void beginTransaction(DeviceAddress newAddress) override
    {
        mutex.lock();
        currentAddress = newAddress;
    }

    //--------------------------------------------------------------------------------------------------
    void endTransaction() override
    {
        mutex.unlock();
    }

    //--------------------------------------------------------------------------------------------------
    bool read(uint8_t *buffer, uint16_t length) override
    {
        auto returnValue = i2c_master_read_from_device(i2cNumber, currentAddress, buffer, length,
                                                       toOsTicks(Timeout));

        bool hasError = returnValue != ESP_OK;

        if (hasError)
            ESP_LOGE(printTag.data(), "reading from device at address: %d returns error: %s",
                     currentAddress, esp_err_to_name(returnValue));

        return !hasError;
    }

    //--------------------------------------------------------------------------------------------------
    bool readFromRegister(uint8_t registerAdress, uint8_t *buffer, uint16_t length) override
    {
        auto returnValue = i2c_master_write_read_device(i2cNumber, currentAddress, &registerAdress,
                                                        1, buffer, length, toOsTicks(Timeout));

        bool hasError = returnValue != ESP_OK;

        if (hasError)
            ESP_LOGE(printTag.data(),
                     "reading register %d from device at address: %d returns error: %s",
                     registerAdress, currentAddress, esp_err_to_name(returnValue));

        return !hasError;
    }

    //--------------------------------------------------------------------------------------------------
    bool readByteFromRegister(uint8_t registerAdress, uint8_t &byte) override
    {
        return readFromRegister(registerAdress, &byte, 1);
    }

    //--------------------------------------------------------------------------------------------------
    bool readWordFromRegister(uint8_t registerAdress, uint16_t &word) override
    {
        bool returnValue = readFromRegister(registerAdress, reinterpret_cast<uint8_t *>(&word), 2);
        swapBytes(word);
        return returnValue;
    }

    //--------------------------------------------------------------------------------------------------
    bool write(const uint8_t *data, uint16_t length) override
    {
        auto returnValue =
            i2c_master_write_to_device(i2cNumber, currentAddress, data, length, toOsTicks(Timeout));

        bool hasError = returnValue != ESP_OK;

        if (hasError)
            ESP_LOGE(printTag.data(), "writing to device at address: %d returns error: %s",
                     currentAddress, esp_err_to_name(returnValue));

        return !hasError;
    }

    //--------------------------------------------------------------------------------------------------
    bool writeByteToRegister(uint8_t registerAdress, const uint8_t byte) override
    {
        uint8_t data[] = {registerAdress, byte};
        return write(data, 2);
    }

    //--------------------------------------------------------------------------------------------------
    bool writeWordToRegister(uint8_t registerAdress, const uint16_t word) override
    {
        uint8_t data[] = {registerAdress, (uint8_t)((word & 0xFF) << 8), (uint8_t)(word >> 8)};
        return write(data, 3);
    }

    //--------------------------------------------------------------------------------------------------
    void swapBytes(uint16_t &val)
    {
        val = ((val & 0xFF) << 8) | (val >> 8);
    }

    static constexpr auto Timeout = 100.0_ms;

private:
    i2c_port_t i2cNumber{};
    DeviceAddress currentAddress{};

    std::string printTag = "[I2cBusaccessor]";

    util::wrappers::Mutex mutex{};
    util::wrappers::BinarySemaphore binarySemaphore{};
    bool errorCondition = false;
};