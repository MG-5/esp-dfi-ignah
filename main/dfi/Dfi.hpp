#pragma once

#include "HttpClient.hpp"
#include "Time.hpp"
#include "pugixml.hpp"
#include "wrappers/Task.hpp"

#include "led/LedControl.hpp"

class Dfi : public util::wrappers::TaskWithMemberFunctionBase
{
public:
    static constexpr auto PrintTag = "[DFI]";
    static constexpr auto MaximumNumberVehiclesRequest = 16;
    static constexpr auto MaximumNumberVehiclesToShow = 8;
    static constexpr auto MaximumNumberOfAdditionalVehicles = 8;

    using BlacklistArray = std::array<std::string_view, 8>;

    struct Station
    {
        uint16_t stationNumber = 0;
        std::string_view stationName = "";
        BlacklistArray blacklist{};
    };

    static constexpr Dfi::Station AmbrosiusplatzRtgStadt{
        7307,                                                                              //
        "Ambrosiusplatz",                                                                  //
        {"Sudenburg", "Reform", "Friedenshöhe", "Magdeburg, Sudenburg, Braunlager Str."}}; //

    struct LocalTransportVehicle
    {
        std::string lineNumber = "";
        std::string directionName = "";
        int delay = 0;
        Time fpTime{};
        int arrivalInMinutes = 0;
    };

    static bool localTransportVehicleSorter(LocalTransportVehicle const &lhs,
                                            LocalTransportVehicle const &rhs)
    {
        return lhs.arrivalInMinutes < rhs.arrivalInMinutes;
    }

    using LocalTransportVehicleArray =
        std::array<LocalTransportVehicle,
                   MaximumNumberVehiclesToShow + MaximumNumberOfAdditionalVehicles>;

    using AdditionalVehicleList = std::vector<LocalTransportVehicle>;

    explicit Dfi(bool &isConnected)
        : TaskWithMemberFunctionBase("dfiTask", 2048, osPriorityNormal3),
          isConnected(isConnected) //
          {};

    std::string_view getStationName()
    {
        return currentStation->stationName;
    }

    const LocalTransportVehicleArray &getVehicles() const
    {
        return vehicleArray;
    }

    void setAdditionalVehicles(AdditionalVehicleList &additionalVehicles);
    void getAdditionalVehicles(AdditionalVehicleList &additionalVehicles);

protected:
    void taskMain(void *) override;

private:
    bool &isConnected;
    const Station *currentStation = &AmbrosiusplatzRtgStadt;

    HttpClient httpClient{MaximumNumberVehiclesRequest};
    pugi::xml_document xmlDocument{};
    LocalTransportVehicleArray vehicleArray{};

    AdditionalVehicleList additionalVehicleList{MaximumNumberOfAdditionalVehicles};

    bool loadXmlFromBuffer();
    void parseXml();

    [[nodiscard]] Time getCurrentLocalTime();
    [[nodiscard]] bool isArrivalTimeInFuture(LocalTransportVehicle &vehicle);
    [[nodiscard]] int calculateArrivalTime(LocalTransportVehicle &vehicle);

    void logVehicles();
};
