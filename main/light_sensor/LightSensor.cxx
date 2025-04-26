#include "LightSensor.hpp"
#include "util/MapValue.hpp"

//--------------------------------------------------------------------------------------------------
void LightSensor::taskMain(void *)
{
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