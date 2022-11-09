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

constexpr Dfi::Station AmbrosiusplatzRtgStadt{7307,                      //
                                              "Ambrosiusplatz -> Stadt", //
                                              {"Sudenburg", "Reform"}};  //

using namespace util::wrappers;

//--------------------------------------------------------------------------------------------------
void Dfi::taskMain(void *)
{
    currentStation = &AmbrosiusplatzRtgStadt;

    initDisplayInterface();

    sync::waitForAll(sync::ConnectedToWifi);
    sync::waitForAll(sync::TimeIsSynchronized);

    constexpr auto TriggerDelay = 60.0_s;

    auto lastWakeTime = xTaskGetTickCount();

    while (true)
    {
        if (!isConnected)
        {
            ESP_LOGW(PrintTag, "No connection. Wait for reconnect.");
            Timebase::printLocaltime();
            sync::waitForAll(sync::ConnectedToWifi);
        }

        Timebase::printLocaltime();

        if (httpClient.requestData(currentStation->stationNumber))
        {
            loadXmlFromBuffer();
            parseXml();

            clearDisplayRam();
            printTitleBar();
            printVehicles();
            renderer.render();
        }

        vTaskDelayUntil(&lastWakeTime, toOsTicks(TriggerDelay));
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
void Dfi::printTitleBar()
{
    renderer.print({0, 0}, currentStation->stationName.data(), Renderer::Alignment::Left);

    std::tm *localTime = Timebase::getLocaltime(Timebase::getCurrentUTC());

    snprintf(printBuffer, PrintBufferSize, "%02d:%02d", localTime->tm_hour, localTime->tm_min);
    renderer.print({296, 0}, printBuffer, Renderer::Alignment::Right, 2);
    renderer.drawHorizontalLine(1, 7);
}

//--------------------------------------------------------------------------------------------------
void Dfi::printVehicles()
{
    size_t pageCounter = 1;

    for (auto &vehicle : vehicleArray)
    {
        if (vehicle.lineNumber == "" && vehicle.directionName == "")
            continue;

        if (pageCounter == 5)
            break;

        try
        {
            // put line number and direction in a string
            snprintf(printBuffer, PrintBufferSize, "%s %s ", vehicle.lineNumber.substr(6).data(),
                     vehicle.directionName.data());

            renderer.print({0, pageCounter}, printBuffer, Renderer::Alignment::Left, 2);
        }
        catch (...)
        {
            ESP_LOGE(PrintTag, "line number is not valid, reject it: %s",
                     vehicle.lineNumber.data());
            continue;
        }

        if (vehicle.arrivalInMinutes == 0)
            renderer.print({296, pageCounter}, "jetzt", Renderer::Alignment::Right, 2);

        else
        {
            snprintf(printBuffer, PrintBufferSize, "%dmin", vehicle.arrivalInMinutes);
            renderer.print({296, pageCounter}, printBuffer, Renderer::Alignment::Right, 2);
        }

        ESP_LOGI(PrintTag, "Linie %s, %s, Abfahrt in (%dmin) %dmin",
                 vehicle.lineNumber.substr(6).c_str(), vehicle.directionName.c_str(), vehicle.delay,
                 vehicle.arrivalInMinutes);

        pageCounter++;
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
}