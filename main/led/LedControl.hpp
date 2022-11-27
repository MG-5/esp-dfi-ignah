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
    Gpio ControlShiftInput{GPIO_NUM_2};
    Gpio ControlShiftClock{GPIO_NUM_3};
    Gpio ControlEnableOutput{GPIO_NUM_4};
    Gpio RegisterClear{GPIO_NUM_5}; // SRCLR -active low
    Gpio RegisterClock{GPIO_NUM_6}; // RCK
    Gpio ShiftDataIn2{GPIO_NUM_7};  // DI for rows 3, 4, 7 and 8
    Gpio ShiftDataIn1{GPIO_NUM_8};  // DI for rows 1, 2, 5 and 6
    Gpio ShiftClock2{GPIO_NUM_9};   // SRCK for rows 3, 4, 7 and 8
    Gpio ShiftClock1{GPIO_NUM_10};  // SRCK for rows 1, 2, 5 and 6

    static constexpr auto PwmPin1 = GPIO_NUM_11;
    static constexpr auto PwmPin2 = GPIO_NUM_12;
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