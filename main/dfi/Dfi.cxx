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

using namespace util::wrappers;

//--------------------------------------------------------------------------------------------------
void Dfi::taskMain(void *)
{
    sync::waitForAll(sync::ConnectedToWifi);
    sync::waitForAll(sync::TimeIsSynchronized);

    constexpr auto TriggerDelay = 60.0_s;

    auto lastWakeTime = xTaskGetTickCount();

    while (true)
    {
        if (!isConnected)
            ESP_LOGW(PrintTag, "No connection. Use old data.");

        else if (httpClient.requestData(currentStation->stationNumber))
            loadXmlFromBuffer();

        // parse XML buffer, regardless of connection state
        // so we can use old data if there is no connection
        parseXml();
        logVehicles();

        vTaskDelayUntil(&lastWakeTime, toOsTicks(TriggerDelay));
    }
}

//--------------------------------------------------------------------------------------------------
bool Dfi::loadXmlFromBuffer()
{
    auto result = xmlDocument.load_buffer(HttpClient::dataBuffer.data(), HttpClient::bufferIndex);

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

    for (auto vehicle = StationTable.child("Journey");
         vehicle && index < MaximumNumberVehiclesToShow; vehicle = vehicle.next_sibling("Journey"))
    {
        LocalTransportVehicle newVehicle{};

        // extract informations from XML
        newVehicle.lineNumber = vehicle.attribute("hafasname").value();
        newVehicle.directionName = vehicle.attribute("dir").value();
        newVehicle.delay = vehicle.attribute("e_delay").as_int();
        newVehicle.fpTime = Time(vehicle.attribute("fpTime").value());
        newVehicle.arrivalInMinutes = calculateArrivalTime(newVehicle);

        if (!isArrivalTimeInFuture(newVehicle))
            continue; // reject it

        bool directionIsInBlacklist = false;
        for (auto &blacklistStation : currentStation->blacklist)
        {
            if (blacklistStation == "")
                break;

            if (newVehicle.directionName == blacklistStation)
            {
                directionIsInBlacklist = true;
                break;
            }
        }

        if (directionIsInBlacklist)
            // tram is in blacklist, reject this
            continue;

        // apply values to array entry
        vehicleArray[index] = newVehicle;

        index++;
    }

    // sort by arrival time
    std::sort(vehicleArray.begin(), vehicleArray.end(), &localTransportVehicleSorter);
}

//--------------------------------------------------------------------------------------------------
Time Dfi::getCurrentLocalTime()
{
    // get computers local time
    std::tm *localTime = Timebase::getLocaltime(Timebase::getCurrentUTC());
    return Time(localTime->tm_hour, localTime->tm_min);
}

//--------------------------------------------------------------------------------------------------
bool Dfi::isArrivalTimeInFuture(LocalTransportVehicle &vehicle)
{
    if (vehicle.lineNumber.empty())
        return false;

    // high difference has high probability
    // that the tram arrival time is before the current time
    return vehicle.arrivalInMinutes < 12 * 60;
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
int Dfi::calculateArrivalTime(LocalTransportVehicle &vehicle)
{
    // calc the difference between two times, it can handle the time crossing at midnight.
    // arrival times that are before the current time will be handled at time occuring next
    // day resulting in high minutes difference, so there is a need to handle this
    Time arrivalTime = vehicle.fpTime + vehicle.delay;
    return Time::getDifferenceInMinutes(getCurrentLocalTime(), arrivalTime);
}