#pragma once

#include "I2cBusAccessor.hpp"
#include "helpers/freertos.hpp"
#include "wrappers/BinarySemaphore.hpp"
#include "wrappers/Mutex.hpp"

#include "driver/i2c_master.h"
#include "esp_log.h"

#include <string>

class EspI2cBusAccessor : public I2cBusAccessor
{
public:
    explicit EspI2cBusAccessor(i2c_port_t i2cNumber, gpio_num_t sdaPin, gpio_num_t sclPin)
        : i2cNumber(i2cNumber)
    {
        printTag = "[I2cBusaccessor port: ";
        printTag += std::to_string(i2cNumber);
        printTag += ")]";

        i2c_master_bus_config_t busConfig = {
            .i2c_port = i2cNumber,
            .sda_io_num = sdaPin,
            .scl_io_num = sclPin,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .flags =
                {
                    .enable_internal_pullup = true,
                    .allow_pd = false,
                },
        };

        ESP_ERROR_CHECK(i2c_new_master_bus(&busConfig, &busHandle));

        // mute spamming nack errors
        esp_log_level_set("i2c.master", ESP_LOG_NONE);
    }

    i2c_master_dev_handle_t addDevice(DeviceAddress deviceAddress) override
    {
        i2c_device_config_t deviceConfig = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = deviceAddress,
            .scl_speed_hz = 400'000,
            .scl_wait_us = 0,
            .flags =
                {
                    .disable_ack_check = false,
                },
        };

        i2c_master_dev_handle_t deviceHandle;

        ESP_ERROR_CHECK(i2c_master_bus_add_device(busHandle, &deviceConfig, &deviceHandle));
        return deviceHandle;
    }

    //--------------------------------------------------------------------------------------------------
    /// @param newAddress 7-bit device address
    void beginTransaction(i2c_master_dev_handle_t deviceHandle) override
    {
        mutex.lock();
        currentDeviceHandle = deviceHandle;
    }

    //--------------------------------------------------------------------------------------------------
    void endTransaction() override
    {
        mutex.unlock();
    }

    //--------------------------------------------------------------------------------------------------
    bool read(uint8_t *buffer, uint16_t length) override
    {
        auto returnValue =
            i2c_master_receive(currentDeviceHandle, buffer, length, toOsTicks(Timeout));

        bool hasError = returnValue != ESP_OK;

        if (hasError)
            ESP_LOGD(printTag.data(), "reading from device returns error: %s",
                     esp_err_to_name(returnValue));

        return !hasError;
    }

    //--------------------------------------------------------------------------------------------------
    bool readFromRegister(uint8_t registerAdress, uint8_t *buffer, uint16_t length) override
    {
        auto returnValue = i2c_master_transmit_receive(currentDeviceHandle, &registerAdress, 1,
                                                       buffer, length, toOsTicks(Timeout));

        bool hasError = returnValue != ESP_OK;

        if (hasError)
            ESP_LOGD(printTag.data(), "reading register %d from device returns error: %s",
                     registerAdress, esp_err_to_name(returnValue));

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
            i2c_master_transmit(currentDeviceHandle, data, length, toOsTicks(Timeout));

        bool hasError = returnValue != ESP_OK;

        if (hasError)
            ESP_LOGD(printTag.data(), "writing to device returns error: %s",
                     esp_err_to_name(returnValue));

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
    i2c_master_bus_handle_t busHandle;
    i2c_port_t i2cNumber{};

    i2c_master_dev_handle_t currentDeviceHandle;

    std::string printTag = "[I2cBusaccessor]";

    util::wrappers::Mutex mutex{};
    util::wrappers::BinarySemaphore binarySemaphore{};
    bool errorCondition = false;
};