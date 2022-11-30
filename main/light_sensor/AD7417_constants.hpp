#pragma once
#include <cstdint>

namespace ad7417_constants
{
enum class Register : uint8_t
{
    TemperatureValue = 0x00,
    Configuration = 0x01,
    THystSetPoint = 0x02,
    TOtiSetPoint = 0x03,
    AdcValue = 0x04,
    Config2 = 0x05
};

namespace TemperatureValue
{
// 10 bits MSB
static constexpr uint16_t Pos = 6;
static constexpr uint16_t ContentMask = 0b1111111111;
} // namespace TemperatureValue

namespace Configuration
{
static constexpr uint8_t ChannelPos = 5;
static constexpr uint8_t ChannelMask = 0b111;

static constexpr uint8_t FaultQueuePos = 3;
static constexpr uint8_t FaultQueueMask = 0b11;

static constexpr uint8_t OtiPolarityBit = 2;
static constexpr uint8_t ComparatorInterruptModeBit = 1;
static constexpr uint8_t ShutdownBit = 0;
} // namespace Configuration

namespace THystSetPoint
{
// 9 bits MSB
static constexpr uint16_t Pos = 7;
static constexpr uint16_t ContentMask = 0b111111111;
} // namespace THystSetPoint

namespace TOtiSetPoint
{
// 9 bits MSB
static constexpr uint16_t Pos = 7;
static constexpr uint16_t ContentMask = 0b111111111;
} // namespace TOtiSetPoint

namespace AdcValue
{
// 10 bits MSB
static constexpr uint16_t Pos = 6;
static constexpr uint16_t ContentMask = 0b1111111111;
} // namespace AdcValue

namespace Config2
{
static constexpr uint16_t ConversionModeBit = 7;
static constexpr uint16_t Test1Bit = 6;
} // namespace Config2

} // namespace ad7417_constants