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

protected:
    void submitImage(const uint8_t *image, size_t length) override;

private:
    Gpio MultiplexerOut1{GPIO_NUM_4};
    Gpio MultiplexerOut2{GPIO_NUM_13};
    Gpio ControlShiftInput{GPIO_NUM_12};
    Gpio ControlShiftClock{GPIO_NUM_14};
    Gpio ControlEnableOutput{GPIO_NUM_27};
    Gpio RegisterClear{GPIO_NUM_26}; // SRCLR -active low
    Gpio RegisterClock{GPIO_NUM_25}; // RCK
    Gpio ShiftDataIn2{GPIO_NUM_33};  // DI for rows 3, 4, 7 and 8
    Gpio ShiftDataIn1{GPIO_NUM_32};  // DI for rows 1, 2, 5 and 6
    Gpio ShiftClock2{GPIO_NUM_35};   // SRCK for rows 3, 4, 7 and 8
    Gpio ShiftClock1{GPIO_NUM_34};   // SRCK for rows 1, 2, 5 and 6

    static constexpr auto PwmPin1 = GPIO_NUM_2;
    static constexpr auto PwmPin2 = GPIO_NUM_15;
    static constexpr auto PwmChannel1 = LEDC_CHANNEL_0;
    static constexpr auto PwmChannel2 = LEDC_CHANNEL_1;

    static constexpr auto PwmMode = LEDC_LOW_SPEED_MODE;
    static constexpr auto PwmResolution = LEDC_TIMER_10_BIT;
    static constexpr auto PwmMaximumDuty = (1 << PwmResolution) - 1;

    void initGpios();

    void initPwm();
    void setPwmDuty(size_t dutyCycle);

    // one bit = one row
    void setTripleControlShiftRegister(uint8_t rows);
    void triggerControlShiftRegisterOutput();
    void shiftControlData(uint8_t byte);

    void clearShiftRegisters();
    void triggerRegisterClock();
    void shiftDualLedData(const size_t startPosition, const uint8_t *image);
    void shiftLedDataRow5(const uint8_t *image);

    void delay200ns();
};