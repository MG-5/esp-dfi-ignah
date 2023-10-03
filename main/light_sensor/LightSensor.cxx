#include "LightSensor.hpp"
#include "util/MapValue.hpp"

//--------------------------------------------------------------------------------------------------
void LightSensor::taskMain(void *)
{
    initI2c();
    powerEnable.init(GPIO_MODE_OUTPUT);

    bool hasPrintedOnce = false;
    auto lastWakeTime = xTaskGetTickCount();
    while (true)
    {
        vTaskDelayUntil(&lastWakeTime, toOsTicks(25.0_Hz));

        if (!isSensorOkay)
        {
            if (reconfigureSensor())
            {
                ESP_LOGI(PrintTag, "Configured light sensor.");
                hasPrintedOnce = false;
                isSensorOkay = true;
            }
            else
            {
                if (!hasPrintedOnce)
                {
                    hasPrintedOnce = true;
                    ESP_LOGE(PrintTag, "Light sensor cannot be configured!");
                }
                continue;
            }
        }

        if (!readSensor())
        {
            ESP_LOGE(PrintTag, "error reading out sensor!");
            isSensorOkay = false;
        }
        else
        {
            updateFastLowpass(filteredValue, (uint16_t)(rawValue), FilterSampleSize);
            ESP_LOGD(PrintTag, "sensor raw value: %d, filtered value: %d", rawValue, filteredValue);

            updatePwm();
        }
    }
}

//--------------------------------------------------------------------------------------------------
void LightSensor::initI2c()
{
    i2c_config_t config{};
    config.mode = I2C_MODE_MASTER;
    config.sda_io_num = SdaPin;
    config.scl_io_num = SclPin;
    config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    config.master.clk_speed = 400'000;

    i2c_param_config(I2cPort, &config);

    ESP_ERROR_CHECK(i2c_driver_install(I2cPort, config.mode, 0, 0, 0));
}

//--------------------------------------------------------------------------------------------------
bool LightSensor::reconfigureSensor()
{
    ESP_LOGD(PrintTag, "restart and reconfigure sensor");
    powerEnable.write(false);
    vTaskDelay(toOsTicks(100.0_ms));
    powerEnable.write(true);
    vTaskDelay(toOsTicks(100.0_ms));

    return sensor.setChannel(AD7417::Channel::Channel1);
}

//--------------------------------------------------------------------------------------------------
bool LightSensor::readSensor()
{
    if (!sensor.wakeUp())
        return false;

    // conversion takes 400µs
    vTaskDelay(toOsTicks(1.0_ms));

    auto optionalResult = sensor.getAdcValue();
    if (!optionalResult)
        return false;

    rawValue = optionalResult.value();

    return sensor.shutdown();
}

//--------------------------------------------------------------------------------------------------
void LightSensor::updatePwm()
{
    constexpr uint16_t InputMaximum = ((1 << Resolution) - 1);

    uint16_t pwmValue = util::mapValue<uint16_t, uint16_t>(0, InputMaximum, settings.pwmMinimum,
                                                           settings.pwmMaximum, filteredValue);

    pwmValue *= settings.pwmGain;

    if (pwmValue > settings.pwmMaximum)
        pwmValue = settings.pwmMaximum;

    ledControl.setPwmDuty(pwmValue);

    ESP_LOGD(PrintTag, "pwm value: %d", pwmValue);
}