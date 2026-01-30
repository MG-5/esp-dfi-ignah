#pragma once

#include "display-renderer/IRenderTarget.hpp"
#include "util/gpio.hpp"

#include "driver/ledc.h"

using util::Gpio;

class LedControl : public IRenderTarget
{
public:
    static constexpr auto Strips = 5;
    static constexpr auto Columns = 120;
    static constexpr auto LedsPerStrip = Columns * 8;
    static constexpr auto TotalLeds = Strips * LedsPerStrip;

    LedControl() = default;

    void init();
    void setPwmDuty(size_t dutyCycle);

protected:
    void submitImage(const uint8_t *image, size_t length) override;

public:
    static constexpr auto PrintTag = "[LedControl]";

    // multiplexer pin will be not used because ESP32-C3 has no enough GPIOs
    // Gpio MultiplexerOut1{--}; // comparator 1 out 2 or comparator 2 out 2
    // Gpio MultiplexerOut2{--}; // comparator 1 out 1 or comparator 2 out 1

    Gpio ControlShiftInput{GPIO_NUM_20};
    Gpio ControlShiftClock{GPIO_NUM_21};
    Gpio ControlEnableOutput{GPIO_NUM_0};
    Gpio RegisterClear{GPIO_NUM_1}; // SRCLR -active low
    Gpio RegisterClock{GPIO_NUM_2}; // RCK
    Gpio ShiftDataIn2{GPIO_NUM_3};  // DI for rows 3, 4, 7 and 8
    Gpio ShiftDataIn1{GPIO_NUM_4};  // DI for rows 1, 2, 5 and 6
    Gpio ShiftClock2{GPIO_NUM_5};   // SRCK for rows 3, 4, 7 and 8
    Gpio ShiftClock1{GPIO_NUM_6};   // SRCK for rows 1, 2, 5 and 6

    static constexpr auto PwmPin = GPIO_NUM_8;
    static constexpr auto PwmChannel = LEDC_CHANNEL_0;

    static constexpr auto PwmMode = LEDC_LOW_SPEED_MODE;

    void initGpios();

    void initPwm();

    // one bit = one row
    void setTripleControlShiftRegister(uint8_t rows);
    void triggerControlShiftRegisterOutput();
    void shiftControlData(uint8_t byte);

    void clearShiftRegisters();
    void triggerRegisterClock();
    void shiftDualLedData(const size_t startPosition, const uint8_t *image);
    void shiftLedDataRow5(const uint8_t *image);

    void delay50ns();
};