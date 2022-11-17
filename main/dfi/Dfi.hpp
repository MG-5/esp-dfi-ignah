#pragma once

#include "HttpClient.hpp"
#include "Time.hpp"
#include "pugixml.hpp"
#include "wrappers/Task.hpp"

#include "display-renderer/Renderer.hpp"
#include "led/LedControl.hpp"

class Dfi : public util::wrappers::TaskWithMemberFunctionBase
{
public:
    static constexpr auto PrintTag = "[DFI]";
    static constexpr auto MaximumNumberVehiclesRequest = 16;
    static constexpr auto MaximumNumberVehiclesToShow = 8;

    using BlacklistArray = std::array<std::string_view, 8>;

    struct Station
    {
        uint16_t stationNumber = 0;
        std::string_view stationName = "";
        BlacklistArray blacklist{};
    };

    struct LocalTransportVehicle
    {
        std::string lineNumber = "";
        std::string directionName = "";
        int delay = 0;
        Time fpTime{};
        int arrivalInMinutes = 0;
    };

    explicit Dfi(bool &isConnected)
        : TaskWithMemberFunctionBase("dfiTask", 2048, osPriorityNormal3),
          isConnected(isConnected) //
          {};

    static bool localTransportVehicleSorter(LocalTransportVehicle const &lhs,
                                            LocalTransportVehicle const &rhs)
    {
        return lhs.arrivalInMinutes < rhs.arrivalInMinutes;
    }

    using LocalTransportVehicleArray =
        std::array<LocalTransportVehicle, MaximumNumberVehiclesToShow>;

protected:
    void taskMain(void *) override;

private:
    const Station *currentStation = nullptr;

    HttpClient httpClient{MaximumNumberVehiclesRequest};
    pugi::xml_document xmlDocument{};
    LocalTransportVehicleArray vehicleArray;

    LedControl ledControl{};
    Renderer renderer{LedControl::Columns, LedControl::Strips, ledControl};

    static constexpr auto PrintBufferSize = 32;
    char printBuffer[PrintBufferSize]{};
    bool &isConnected;

    bool loadXmlFromBuffer();
    void parseXml();

    void renderTitleBar(bool showDoublePoint);
    void renderVehicles(bool showCurrentVehicle);
    void logVehicles();

    void initDisplayInterface();
    void clearDisplayRam();
};
