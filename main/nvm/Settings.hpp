#pragma once
#include "driver/ledc.h"
#include <string>

#include "NonVolatileMemory.hpp"
#include "wrappers/Task.hpp"

using namespace util::wrappers;

class Settings : public TaskWithMemberFunctionBase
{
public:
    static constexpr auto PrintTag = "[Settings]";

    explicit Settings(NonVolatileMemory &nvm)
        : TaskWithMemberFunctionBase("settingsTask", 2048, osPriorityAboveNormal2), //
          nvm(nvm){};

    static constexpr auto StationNumberName = "stationNumber";
    static constexpr size_t StationNumberDefault = 7307;

    static constexpr auto StationNameName = "stationName";
    static constexpr auto StationNameDefault = "Ambrosiusplatz";

    static constexpr auto StationBlocklistName = "stationBlcklst";
    static constexpr auto StationBlocklistDefault =
        "Sudenburg;Reform;Friedenshöhe;Magdeburg, Sudenburg, Braunlager Str.";

    static constexpr auto PwmResolution = LEDC_TIMER_10_BIT;
    static constexpr auto PwmMaximumDuty = (1 << PwmResolution) - 1;

    static constexpr auto PwmMinimumName = "pwmMinimum";
    static constexpr size_t PwmMinimumDefault = 100;

    static constexpr auto PwmMaximumName = "pwmMaximum";
    static constexpr size_t PwmMaximumDefault = 0.8 * PwmMaximumDuty;

    static constexpr auto PwmGainName = "pwmGain";
    static constexpr auto PwmGainDefault = 1.0f;

    size_t stationNumber = StationNumberDefault;
    std::string stationName = StationNameDefault;
    std::string stationBlocklist = StationBlocklistDefault;
    size_t pwmMinimum = PwmMinimumDefault;
    size_t pwmMaximum = PwmMaximumDefault;
    float pwmGain = PwmGainDefault;

    template <typename T>
    void updateValue(const char *name, T &value, const T newValue)
    {
        if (value == newValue)
            return;

        value = newValue;

        if constexpr (std::is_same_v<float, T>)
        {
            ESP_LOGI(PrintTag, "Updating %s to %f", name, value);

            nvm.updateValue(name, static_cast<uint32_t>(value * 1000));
        }
        else if constexpr (std::is_same_v<std::string, T>)
        {
            ESP_LOGI(PrintTag, "Updating %s to %s", name, value);
            nvm.updateValue(name, value.c_str());
        }
        else
        {
            ESP_LOGI(PrintTag, "Updating %s to %du", name, value);
            nvm.updateValue(name, value);
        }
    }

protected:
    void taskMain(void *)
    {
        sync::waitForAll(sync::NvmInitialized | sync::LedDriverStarted);
        loadValues();
        sync::signal(sync::ConfigurationLoaded);
    }

private:
    NonVolatileMemory &nvm;

    void loadValues()
    {
        ESP_LOGI(PrintTag, "Loading values from NVS ... ");

        nvm.read(StationNumberName, stationNumber);
        nvm.read(StationNameName, stationName);
        nvm.read(StationBlocklistName, stationBlocklist);
        nvm.read(PwmMinimumName, pwmMinimum);
        nvm.read(PwmMaximumName, pwmMaximum);
        nvm.read(PwmGainName, pwmGain);
    }
};
