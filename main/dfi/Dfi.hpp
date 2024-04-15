#pragma once

#include "HttpClient.hpp"
#include "Time.hpp"
#include "pugixml.hpp"
#include "wrappers/Task.hpp"

#include "led/LedControl.hpp"
#include "nvm/Settings.hpp"

class Dfi : public util::wrappers::TaskWithMemberFunctionBase
{
public:
    static constexpr auto PrintTag = "[DFI]";
    static constexpr auto MaximumNumberVehiclesRequest = 16;
    static constexpr auto MaximumNumberVehiclesToShow = 8;
    static constexpr auto MaximumNumberOfAdditionalVehicles = 8;

    Dfi(bool &isConnected, Settings &settings)
        : TaskWithMemberFunctionBase("dfiTask", 2048, osPriorityAboveNormal),
          isConnected(isConnected), //
          settings(settings)        //
          {};

    struct LocalTransportVehicle
    {
        std::string lineNumber = "";
        std::string directionName = "";
        int delay = 0;
        Time fpTime{};
        int arrivalInMinutes = 0;
    };

    using BlocklistArray = std::array<std::string, 8>;

    using LocalTransportVehicleArray =
        std::array<LocalTransportVehicle,
                   MaximumNumberVehiclesToShow + MaximumNumberOfAdditionalVehicles>;

    using AdditionalVehicleList = std::vector<LocalTransportVehicle>;

    static bool localTransportVehicleSorter(LocalTransportVehicle const &lhs,
                                            LocalTransportVehicle const &rhs)
    {
        return lhs.arrivalInMinutes < rhs.arrivalInMinutes;
    }

    const LocalTransportVehicleArray &getVehicles() const
    {
        return vehicleArray;
    }

    const std::string &getStationName() const
    {
        return settings.stationName;
    }

    const BlocklistArray &getBlocklist() const
    {
        return blocklist;
    }

    void updateBlocklist();

    void setAdditionalVehicles(AdditionalVehicleList &additionalVehicles);
    void getAdditionalVehicles(AdditionalVehicleList &additionalVehicles);

protected:
    void taskMain(void *) override;

private:
    bool &isConnected;
    Settings &settings;

    HttpClient httpClient{MaximumNumberVehiclesRequest};
    pugi::xml_document xmlDocument{};
    LocalTransportVehicleArray vehicleArray{};
    BlocklistArray blocklist{};

    AdditionalVehicleList additionalVehicleList{MaximumNumberOfAdditionalVehicles};

    bool loadXmlFromBuffer();
    void parseXml();

    bool isDirectionInBlacklist(const std::string &directionName);
    void replaceSubstringsInDirectionName(std::string &directionName);

    [[nodiscard]] Time getCurrentLocalTime();
    [[nodiscard]] bool isArrivalTimeInFuture(LocalTransportVehicle &vehicle);
    [[nodiscard]] int calculateArrivalTime(LocalTransportVehicle &vehicle);

    void logVehicles();
};
