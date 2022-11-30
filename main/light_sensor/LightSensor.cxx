#include "LightSensor.hpp"

//--------------------------------------------------------------------------------------------------
void LightSensor::taskMain(void *)
{
    initI2c();
    powerEnable.init(GPIO_MODE_OUTPUT);

    auto lastWakeTime = xTaskGetTickCount();
    while (true)
    {
        vTaskDelayUntil(&lastWakeTime, toOsTicks(100.0_Hz));

        if (!isSensorOkay)
        {
            if (reconfigureSensor())
                isSensorOkay = true;
            else
                continue;
        }

        if (!readSensor())
        {
            ESP_LOGE(PrintTag, "error reading out sensor!");
            isSensorOkay = false;
        }
        else
        {
            updateFastLowpass(filterdValue, rawValue, FilterSampleSize);
            ESP_LOGI(PrintTag, "sensor raw value: %d, filtered value: %d", rawValue, filterdValue);
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
    ESP_LOGI(PrintTag, "restart and reconfigure sensor");
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