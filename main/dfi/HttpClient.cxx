#include "HttpClient.hpp"
#include "esp_log.h"

#include <string>

namespace
{
constexpr auto PrintTag = "[HttpClient]";

constexpr auto Hostname = "reiseauskunft.insa.de";
constexpr auto Path = "/bin/stboard.exe/dn";
} // namespace

size_t HttpClient::bufferIndex = 0;
std::array<char, MaximumBufferSize> HttpClient::dataBuffer;

bool HttpClient::requestData(uint32_t stationNumber)
{
    bufferIndex = 0;
    dataBuffer.fill(0);

    std::string queryString = "";
    queryString += "boardType=dep";
    queryString += "&maxJourneys=" + std::to_string(NumberOfJourneys);
    queryString += "&L=vs_java3";
    queryString += "&start=yes";
    queryString += "&input=" + std::to_string(stationNumber);

    esp_http_client_config_t config = {
        .host = Hostname,
        .path = Path,
        .query = queryString.data(),
        .disable_auto_redirect = true,
        .event_handler = HttpClient::httpEventHandler,
    };

    ESP_LOGI(PrintTag, "request INSA data");
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);
    esp_http_client_cleanup(client);

    if (err != ESP_OK)
    {
        ESP_LOGE(PrintTag, "Error perform http request %s", esp_err_to_name(err));
        return false;
    }

    ESP_LOGI(PrintTag, "HTTP Status = %d, data length = %d",
             esp_http_client_get_status_code(client), bufferIndex);
    return true;
}

esp_err_t HttpClient::httpEventHandler(esp_http_client_event_t *event)
{
    switch (event->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(PrintTag, "HTTP_EVENT_ERROR");
        break;

    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(PrintTag, "HTTP_EVENT_ON_CONNECTED");
        break;

    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(PrintTag, "HTTP_EVENT_HEADER_SENT");
        break;

    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(PrintTag, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", event->header_key,
                 event->header_value);
        break;

    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(PrintTag, "HTTP_EVENT_ON_DATA, len=%d", event->data_len);

        if (event->data_len + bufferIndex > MaximumBufferSize)
        {
            ESP_LOGE(PrintTag, "content to big for buffer, reject it");
            break;
        }
        memcpy(HttpClient::dataBuffer.data() + bufferIndex, event->data, event->data_len);
        bufferIndex += event->data_len;

        ESP_LOGD(PrintTag, "copied %d bytes to buffer, offset is now at %d", event->data_len,
                 bufferIndex);
        break;

    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(PrintTag, "HTTP_EVENT_ON_FINISH");
        // Response is accumulated in output_buffer. Uncomment the below line to print the
        // accumulated response
        ESP_LOGD(PrintTag, "buffer contains %d bytes data", bufferIndex);
        // ESP_LOG_BUFFER_CHAR(Tag, output_buffer, bufferIndex);

        break;

    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(PrintTag, "HTTP_EVENT_DISCONNECTED");
        break;

    case HTTP_EVENT_REDIRECT:
        ESP_LOGD(PrintTag, "HTTP_EVENT_REDIRECT");
        return esp_http_client_set_redirection(event->client);
        break;
    }
    return ESP_OK;
}