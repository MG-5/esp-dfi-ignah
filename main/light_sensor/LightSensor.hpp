#pragma once

#include "AD7417.hpp"
#include "led/LedControl.hpp"
#include "nvm/Settings.hpp"
#include "util/gpio.hpp"
#include "wrappers/Task.hpp"

class LightSensor : public util::wrappers::TaskWithMemberFunctionBase
{

public:
    LightSensor(LedControl &ledControl, Settings &settings)
        : TaskWithMemberFunctionBase("lightSensorTask", 1024, osPriorityBelowNormal3), //
          ledControl(ledControl),                                                      //
          settings(settings)                                                           //
    {};

    static constexpr auto I2cPort = I2C_NUM_0;
    static constexpr auto SdaPin = GPIO_NUM_21;
    static constexpr auto SclPin = GPIO_NUM_22;

protected:
    void taskMain(void *) override;

private:
    static constexpr auto PrintTag = "[LightSensor]";
    static constexpr auto Resolution = 10;
    static constexpr auto FilterSampleSize = 64;

    LedControl &ledControl;
    Settings &settings;

    EspI2cBusAccessor espI2cBusAccessor{I2cPort, SdaPin, SclPin};
    AD7417 sensor{espI2cBusAccessor, 0b111};
    util::Gpio powerEnable{GPIO_NUM_23};

    uint16_t rawValue = 0;
    uint16_t filteredValue = (1 << Resolution) / 2; // init with value lay in the middle
    bool isSensorOkay = false;

    bool reconfigureSensor();
    bool readSensor();
    void updatePwm();

    template <class T>
    static void updateFastLowpass(T &oldValue, const T newSample, const uint8_t sampleCount)
    {
        oldValue += (newSample - oldValue) / static_cast<float>(sampleCount);
    }
};