#pragma once

#include "esp_event.h"
#include "esp_http_client.h"

#include <array>

constexpr auto MaximumBufferSize = 25'000;

class HttpClient
{

public:
    HttpClient(uint8_t numberOfJourneys) : NumberOfJourneys(numberOfJourneys){};

    const uint8_t NumberOfJourneys;

    static esp_err_t httpEventHandler(esp_http_client_event_t *event);

    bool requestData(uint16_t stationNumber);

    static std::array<char, MaximumBufferSize> dataBuffer;

    static size_t bufferIndex;
};
