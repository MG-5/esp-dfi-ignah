#include "Dfi.hpp"
#include "Timebase.hpp"
#include "helpers/freertos.hpp"
#include "sync.hpp"

#include "esp_log.h"
#include "esp_sntp.h"

#include <cmath>
#include <codecvt>
#include <ctime>
#include <locale>
#include <string>
#include <sys/time.h>

constexpr Dfi::Station AmbrosiusplatzRtgStadt{
    7307,                                                                              //
    "Ambrosiusplatz",                                                                  //
    {"Sudenburg", "Reform", "Friedenshöhe", "Magdeburg, Sudenburg, Braunlager Str."}}; //

using namespace util::wrappers;

//--------------------------------------------------------------------------------------------------
void Dfi::taskMain(void *)
{
    currentStation = &AmbrosiusplatzRtgStadt;

    initDisplayInterface();
    ledControl.init();

    sync::waitForAll(sync::ConnectedToWifi);
    sync::waitForAll(sync::TimeIsSynchronized);

    constexpr auto TaskDelay = 1.0_s;
    constexpr auto TriggerDelay = 60.0_s;

    auto lastWakeTime = xTaskGetTickCount();
    bool blinkState = true;
    size_t timeCounter = TriggerDelay.getMagnitude();

    while (true)
    {
        clearDisplayRam();
        renderTitleBar(blinkState);

        if (++timeCounter >= (TriggerDelay / TaskDelay).getMagnitude())
        {
            timeCounter = 0;

            if (!isConnected)
                ESP_LOGW(PrintTag, "No connection. Wait for reconnect.");

            else if (httpClient.requestData(currentStation->stationNumber))
                loadXmlFromBuffer();

            // parse XML buffer, regardless of connection state
            // so we can use old data if there is no connection
            parseXml();
            logVehicles();
        }

        renderVehicles(blinkState);
        renderer.render();

        blinkState = !blinkState;

        vTaskDelayUntil(&lastWakeTime, toOsTicks(TaskDelay));
    }
}

//--------------------------------------------------------------------------------------------------
bool Dfi::loadXmlFromBuffer()
{
    auto result =
        xmlDocument.load_buffer_inplace(HttpClient::dataBuffer.data(), HttpClient::bufferIndex);

    if (!result)
    {
        ESP_LOGE(PrintTag, "XML cannot be read from buffer.");
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
void Dfi::parseXml()
{
    vehicleArray.fill(LocalTransportVehicle{});

    const auto StationTable = xmlDocument.child("StationTable");
    size_t index = 0;

    // get computers local time
    std::tm *localTime = Timebase::getLocaltime(Timebase::getCurrentUTC());

    Time currentTime(localTime->tm_hour, localTime->tm_min);

    for (auto vehicle = StationTable.child("Journey");
         vehicle && index < MaximumNumberVehiclesToShow; vehicle = vehicle.next_sibling("Journey"))
    {
        // extract informations from XML
        std::string currentLineNumber = vehicle.attribute("hafasname").value();
        std::string currentDirectionName = vehicle.attribute("dir").value();

        vehicleArray[index].delay = vehicle.attribute("e_delay").as_int();
        vehicleArray[index].fpTime = Time(vehicle.attribute("fpTime").value());

        Time arrivalTime = vehicleArray[index].fpTime + vehicleArray[index].delay;

        // calc the difference between two times, it can handle the time crossing at midnight.
        // arrival times that are before the current time will be handled at time occuring next day
        // resulting in high minutes difference, so there is a need to handle this
        uint16_t arrivalInMinutes = Time::getDifferenceInMinutes(currentTime, arrivalTime);

        if (arrivalInMinutes >= 12 * 60)
        {
            // due the high difference the probability that the tram arrival time is before the
            // current time is high so we guess the tram is gone, so reject it
            continue;
        }

        bool directionIsInBlacklist = false;
        for (auto &blacklistStation : currentStation->blacklist)
        {
            if (blacklistStation == "")
                break;

            if (currentDirectionName == blacklistStation)
            {
                directionIsInBlacklist = true;
                break;
            }
        }

        if (directionIsInBlacklist)
            // tram is in blacklist, reject this
            continue;

        // apply values to array entry
        vehicleArray[index].arrivalInMinutes = arrivalInMinutes;
        vehicleArray[index].lineNumber = currentLineNumber;
        vehicleArray[index].directionName = currentDirectionName;

        index++;
    }

    // sort by arrival time
    std::sort(vehicleArray.begin(), vehicleArray.end(), &localTransportVehicleSorter);
}

//--------------------------------------------------------------------------------------------------
void Dfi::renderTitleBar(bool showDoublePoint)
{
    renderer.print({0, 0}, currentStation->stationName.data());

    std::tm *localTime = Timebase::getLocaltime(Timebase::getCurrentUTC());

    const char *clockFormat = showDoublePoint ? "%02d:%02d" : "%02d %02d";

    snprintf(printBuffer, PrintBufferSize, clockFormat, localTime->tm_hour, localTime->tm_min);
    renderer.print({LedControl::Columns, 0}, printBuffer, Renderer::Alignment::Right);
}

//--------------------------------------------------------------------------------------------------
void Dfi::renderVehicles(bool showCurrentVehicle)
{
    size_t pageCounter = 1;

    for (auto &vehicle : vehicleArray)
    {
        if (vehicle.lineNumber == "" && vehicle.directionName == "")
            continue;

        if (pageCounter == LedControl::Strips)
            break;

        size_t lineNumberPixelWidth = 0;
        const bool IsTrainAtStation = vehicle.arrivalInMinutes == 0;

        try
        {
            // print line number
            snprintf(printBuffer, PrintBufferSize, "%s ", vehicle.lineNumber.substr(6).data());
            lineNumberPixelWidth = renderer.print({0, pageCounter}, printBuffer);
        }
        catch (...)
        {
            ESP_LOGE(PrintTag, "line number is not valid, reject it: %s",
                     vehicle.lineNumber.data());
            continue;
        }

        // print line direction, blinking when train is at station
        if (!IsTrainAtStation || showCurrentVehicle)
            renderer.print({lineNumberPixelWidth, pageCounter}, vehicle.directionName.data());

        if (!IsTrainAtStation)
        {
            // print arrival time in minutes
            snprintf(printBuffer, PrintBufferSize, "%dmin", vehicle.arrivalInMinutes);
            renderer.print({LedControl::Columns, pageCounter}, printBuffer,
                           Renderer::Alignment::Right);
        }

        pageCounter++;
    }
}

//--------------------------------------------------------------------------------------------------
void Dfi::logVehicles()
{
    for (auto &vehicle : vehicleArray)
    {
        if (vehicle.lineNumber == "" && vehicle.directionName == "")
            continue;

        const char *lineNumber;

        try
        {
            lineNumber = vehicle.lineNumber.substr(6).data();
        }

        catch (const std::exception &e)
        {
            ESP_LOGE(PrintTag, "line number is not valid, reject it: %s",
                     vehicle.lineNumber.data());
            continue;
        }

        ESP_LOGI(PrintTag, "Linie %s, %s, Abfahrt in (%d) %dmin", lineNumber,
                 vehicle.directionName.data(), vehicle.delay, vehicle.arrivalInMinutes);
    }
}

//--------------------------------------------------------------------------------------------------
void Dfi::initDisplayInterface()
{
    std::setlocale(LC_ALL, "CP1252");
}

//--------------------------------------------------------------------------------------------------
void Dfi::clearDisplayRam()
{
    renderer.clearAll();
}