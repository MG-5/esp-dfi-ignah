#pragma once
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

    size_t stationNumber = StationNumberDefault;
    std::string stationName = StationNameDefault;
    std::string stationBlocklist = StationBlocklistDefault;

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
    }
};
