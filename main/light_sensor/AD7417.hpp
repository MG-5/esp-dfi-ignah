#pragma once

#include "AD7417_constants.hpp"
#include "EspI2cAccessor.hpp"
#include <optional>

using namespace ad7417_constants;

class AD7417
{
public:
    AD7417(I2cBusAccessor &i2cBusAccessor, I2cBusAccessor::DeviceAddress chipSelectBits)
        : i2cBusAccessor(i2cBusAccessor),
          deviceAddress(BaseAddress | (chipSelectBits & ChipSelectMask)){};

    enum class Mode : uint8_t
    {
        Normal,
        Shutdown
    };

    enum class ConvstPinMode : uint8_t
    {
        NoUse,
        Use
    };

    enum class Channel : uint8_t
    {
        Temperature,
        Channel1,
        Channel2,
        Channel3,
        Channel4
    };

    enum class ComparatorInterruptMode : uint8_t
    {
        Comparator,
        Interrupt
    };

    std::optional<uint16_t> getTemperatureValue();
    std::optional<uint8_t> getConfiguration();
    std::optional<uint16_t> getTHystSetPoint();
    std::optional<uint16_t> getTOtiSetPoint();
    std::optional<uint8_t> getConfig2();
    std::optional<uint16_t> getAdcValue();

    bool setConfiguration(uint8_t byte);
    bool setConfig2(uint8_t byte);
    bool setChannel(Channel newChannel);
    bool setMode(Mode newMode);
    bool setOtiPolarity(bool isActiveHigh);
    bool setOtiOutput(ComparatorInterruptMode newMode);
    bool setFaultQueue(uint8_t size);
    bool setConvstPin(ConvstPinMode newMode);
    bool setTHystSetPoint(uint16_t setPoint);
    bool setTOtiSetPoint(uint16_t setPoint);
    bool setI2cFilters(bool enable);

    bool shutdown();
    bool wakeUp();

private:
    I2cBusAccessor &i2cBusAccessor;
    I2cBusAccessor::DeviceAddress deviceAddress = BaseAddress; // 7-bit

    static constexpr auto BaseAddress = 0b0101 << 3;
    static constexpr auto ChipSelectMask = 0b111;

    bool readByteFromRegister(Register registerAddress, uint8_t &byte);
    bool readWordFromRegister(Register registerAddress, uint16_t &word);

    bool writeByteToRegister(Register registerAddress, uint8_t byte);
    bool writeWordToRegister(Register registerAddress, uint16_t word);
};
