#include "AD7417.hpp"

#include <algorithm>

using namespace ad7417_constants;

//--------------------------------------------------------------------------------------------------
std::optional<uint16_t> AD7417::getTemperatureValue()
{
    uint16_t value;
    bool sucess = readWordFromRegister(Register::TemperatureValue, value);

    if (!sucess)
        return {};

    value >>= TemperatureValue::Pos;
    return std::optional{value};
}

//--------------------------------------------------------------------------------------------------
std::optional<uint8_t> AD7417::getConfiguration()
{
    uint8_t value;
    bool sucess = readByteFromRegister(Register::Configuration, value);

    return sucess ? std::optional{value} : std::nullopt;
}

//--------------------------------------------------------------------------------------------------
std::optional<uint16_t> AD7417::getTHystSetPoint()
{
    uint16_t value;
    bool sucess = readWordFromRegister(Register::THystSetPoint, value);

    if (!sucess)
        return {};

    value >>= THystSetPoint::Pos;
    return std::optional{value};
}

//--------------------------------------------------------------------------------------------------
std::optional<uint16_t> AD7417::getTOtiSetPoint()
{
    uint16_t value;
    bool sucess = readWordFromRegister(Register::TOtiSetPoint, value);

    if (!sucess)
        return {};

    value >>= TOtiSetPoint::Pos;
    return std::optional{value};
}

//--------------------------------------------------------------------------------------------------
std::optional<uint8_t> AD7417::getConfig2()
{
    uint8_t value;
    bool sucess = readByteFromRegister(Register::Config2, value);

    return sucess ? std::optional{value} : std::nullopt;
}

//--------------------------------------------------------------------------------------------------
std::optional<uint16_t> AD7417::getAdcValue()
{
    uint16_t value;
    bool sucess = readWordFromRegister(Register::AdcValue, value);

    if (!sucess)
        return {};

    value >>= AdcValue::Pos;
    return std::optional{value};
}

//--------------------------------------------------------------------------------------------------
bool AD7417::setConfiguration(uint8_t byte)
{
    return writeByteToRegister(Register::Configuration, byte);
}

//--------------------------------------------------------------------------------------------------
bool AD7417::setConfig2(uint8_t byte)
{
    return writeByteToRegister(Register::Config2, byte);
}

//--------------------------------------------------------------------------------------------------
bool AD7417::setChannel(Channel newChannel)
{
    auto result = getConfiguration();
    if (!result)
        return false;

    uint8_t config = result.value();
    config &= ~(Configuration::ChannelMask << Configuration::ChannelPos);
    config |= ((uint8_t)newChannel & Configuration::ChannelMask) << Configuration::ChannelPos;

    return setConfiguration(config);
}

//--------------------------------------------------------------------------------------------------
bool AD7417::setMode(Mode newMode)
{
    auto result = getConfiguration();
    if (!result)
        return false;

    uint8_t config = result.value();
    config &= ~(1 << Configuration::ShutdownBit);
    config |= ((uint8_t)newMode & 1) << Configuration::ShutdownBit;

    return setConfiguration(config);
}

//--------------------------------------------------------------------------------------------------
bool AD7417::setOtiPolarity(bool isActiveHigh)
{
    auto result = getConfiguration();
    if (!result)
        return false;

    uint8_t config = result.value();
    config &= ~(1 << Configuration::OtiPolarityBit);
    config |= (isActiveHigh ? 1 : 0) << Configuration::OtiPolarityBit;

    return setConfiguration(config);
}

//--------------------------------------------------------------------------------------------------
bool AD7417::setOtiOutput(ComparatorInterruptMode newMode)
{
    auto result = getConfiguration();
    if (!result)
        return false;

    uint8_t config = result.value();
    config &= ~(1 << Configuration::ComparatorInterruptModeBit);
    config |= ((uint8_t)newMode & 1) << Configuration::ComparatorInterruptModeBit;

    return setConfiguration(config);
}

//--------------------------------------------------------------------------------------------------
bool AD7417::setFaultQueue(uint8_t size)
{
    if (size == 3 || size == 5)
        return false;

    auto result = getConfiguration();
    if (!result)
        return false;

    std::clamp<uint8_t>(size, 1, 6);

    uint8_t config = result.value();
    config &= ~(Configuration::FaultQueueMask << Configuration::FaultQueuePos);
    config |= ((size >> 1) & Configuration::FaultQueueMask) << Configuration::FaultQueuePos;

    return setConfiguration(config);
}

//--------------------------------------------------------------------------------------------------
bool AD7417::setConvstPin(ConvstPinMode newMode)
{
    auto result = getConfig2();
    if (!result)
        return false;

    uint8_t config = result.value();
    config &= ~(1 << Config2::ConversionModeBit);
    config |= ((uint8_t)newMode & 1) << Config2::ConversionModeBit;

    return setConfig2(config);
}

//--------------------------------------------------------------------------------------------------
bool AD7417::setTHystSetPoint(uint16_t setPoint)
{
    setPoint <<= THystSetPoint::Pos;
    return writeWordToRegister(Register::THystSetPoint, setPoint);
}

//--------------------------------------------------------------------------------------------------
bool AD7417::setTOtiSetPoint(uint16_t setPoint)
{
    setPoint <<= TOtiSetPoint::Pos;
    return writeWordToRegister(Register::TOtiSetPoint, setPoint);
}

//--------------------------------------------------------------------------------------------------
bool AD7417::setI2cFilters(bool enable)
{
    auto result = getConfig2();
    if (!result)
        return false;

    uint8_t config = result.value();
    config &= ~(1 << Config2::Test1Bit);
    config |= (enable ? 1 : 0) << Config2::Test1Bit;

    return setConfig2(config);
}

//--------------------------------------------------------------------------------------------------
bool AD7417::shutdown()
{
    return setMode(Mode::Shutdown);
}

//--------------------------------------------------------------------------------------------------
bool AD7417::wakeUp()
{
    return setMode(Mode::Normal);
}

//--------------------------------------------------------------------------------------------------
bool AD7417::readByteFromRegister(Register registerAddress, uint8_t &byte)
{
    i2cBusAccessor.beginTransaction(deviceAddress);
    bool sucess = i2cBusAccessor.readByteFromRegister(static_cast<uint8_t>(registerAddress), byte);
    i2cBusAccessor.endTransaction();

    return sucess;
}

//--------------------------------------------------------------------------------------------------
bool AD7417::readWordFromRegister(Register registerAddress, uint16_t &word)
{
    i2cBusAccessor.beginTransaction(deviceAddress);
    bool sucess = i2cBusAccessor.readWordFromRegister(static_cast<uint8_t>(registerAddress), word);
    i2cBusAccessor.endTransaction();

    return sucess;
}

//--------------------------------------------------------------------------------------------------
bool AD7417::writeByteToRegister(Register registerAddress, uint8_t byte)
{
    i2cBusAccessor.beginTransaction(deviceAddress);
    bool sucess = i2cBusAccessor.writeByteToRegister(static_cast<uint8_t>(registerAddress), byte);
    i2cBusAccessor.endTransaction();

    return sucess;
}

//--------------------------------------------------------------------------------------------------
bool AD7417::writeWordToRegister(Register registerAddress, uint16_t word)
{
    i2cBusAccessor.beginTransaction(deviceAddress);
    bool sucess = i2cBusAccessor.writeWordToRegister(static_cast<uint8_t>(registerAddress), word);
    i2cBusAccessor.endTransaction();

    return sucess;
}