#include "RestApiHandlers.hpp"
#include "RestServer.hpp"
#include "dfi/Timebase.hpp"
#include "wifi/Wireless.hpp"

#include "cJSON.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_wifi.h"
#include <fcntl.h>

//--------------------------------------------------------------------------------------------------
esp_err_t RestApiHandlers::commonGetHandler(httpd_req_t *req)
{
    std::string filePath;

    auto serverInstance = reinterpret_cast<RestServer *>(req->user_ctx);
    filePath = serverInstance->basePath;

    if (req->uri[strlen(req->uri) - 1] == '/')
        filePath += "/index.html";

    else
        filePath += req->uri;

    ESP_ERROR_CHECK(RestServer::setContentTypeFromFile(req, filePath));

    if (RestServer::UseGzipCompression)
    {
        httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
        filePath += ".gz";
    }

    int fileStreamId = open(filePath.data(), O_RDONLY, 0);
    if (fileStreamId == -1)
    {
        ESP_LOGW(PrintTag, "Failed to open file: %s, return index.html instead", filePath.data());
        filePath = serverInstance->basePath;
        filePath += "/index.html";
        ESP_ERROR_CHECK(RestServer::setContentTypeFromFile(req, filePath));

        if (RestServer::UseGzipCompression)
            filePath += ".gz";

        fileStreamId = open(filePath.data(), O_RDONLY, 0);
        if (fileStreamId == -1)
        {
            ESP_LOGE(PrintTag, "index.html does not exist!");
            httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Failed to find index.html file");
            return ESP_FAIL;
        }
    }

    size_t readBytes;
    do
    {
        // Read file in chunks into the scratch buffer
        readBytes =
            read(fileStreamId, serverInstance->scratchBuffer, RestServer::ScratchBufferSize);
        if (readBytes == -1)
            ESP_LOGE(PrintTag, "Failed to read file: %s", filePath.data());

        else if (readBytes > 0)
        {
            // Send the buffer contents as HTTP response chunk
            if (httpd_resp_send_chunk(req, serverInstance->scratchBuffer, readBytes) != ESP_OK)
            {
                close(fileStreamId);
                ESP_LOGE(PrintTag, "Failed to send file %s", filePath.data());
                httpd_resp_sendstr_chunk(req, nullptr); // Abort sending file
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                return ESP_FAIL;
            }
        }
    } while (readBytes > 0);

    close(fileStreamId);
    ESP_LOGI(PrintTag, "Sended file: %s", filePath.data());

    // Respond with an empty chunk to signal HTTP response completion
    httpd_resp_send_chunk(req, nullptr, 0);
    return ESP_OK;
}

//--------------------------------------------------------------------------------------------------
esp_err_t RestApiHandlers::systemInfoGetHandler(httpd_req_t *req)
{
    ESP_LOGI(PrintTag, "systemInfoGetHandler");
    httpd_resp_set_type(req, "application/json");

    cJSON *jsonRoot = cJSON_CreateObject();
    esp_chip_info_t chipInfo;
    esp_chip_info(&chipInfo);
    const esp_app_desc_t *appDesc = esp_ota_get_app_description();

    cJSON_AddStringToObject(jsonRoot, "projectName", appDesc->project_name);
    cJSON_AddStringToObject(jsonRoot, "projectVersion", appDesc->version);
    cJSON_AddStringToObject(jsonRoot, "compileDate", appDesc->date);
    cJSON_AddStringToObject(jsonRoot, "idfVersion", IDF_VER);
    cJSON_AddNumberToObject(jsonRoot, "model", chipInfo.model);
    cJSON_AddNumberToObject(jsonRoot, "cores", chipInfo.cores);

    std::string_view jsonData = cJSON_Print(jsonRoot);
    httpd_resp_sendstr(req, jsonData.data());
    cJSON_Delete(jsonRoot);
    return ESP_OK;
}

//--------------------------------------------------------------------------------------------------
esp_err_t RestApiHandlers::systemClockGetHandler(httpd_req_t *req)
{
    ESP_LOGI(PrintTag, "systemClockGetHandler");
    httpd_resp_set_type(req, "application/json");

    auto serverInstance = reinterpret_cast<RestServer *>(req->user_ctx);
    auto localTime = Timebase::getLocaltime(Timebase::getCurrentUTC());
    snprintf(serverInstance->scratchBuffer, RestServer::ScratchBufferSize, "%02d:%02d",
             localTime->tm_hour, localTime->tm_min);

    cJSON *jsonRoot = cJSON_CreateObject();
    cJSON_AddStringToObject(jsonRoot, "clock", serverInstance->scratchBuffer);
    cJSON_AddStringToObject(jsonRoot, "timezone", Timebase::Timezone);

    std::string_view jsonData = cJSON_Print(jsonRoot);
    httpd_resp_sendstr(req, jsonData.data());
    cJSON_Delete(jsonRoot);
    return ESP_OK;
}

//--------------------------------------------------------------------------------------------------
esp_err_t RestApiHandlers::wifiStationGetHandler(httpd_req_t *req)
{
    ESP_LOGI(PrintTag, "wifiStationGetHandler");
    httpd_resp_set_type(req, "application/json");

    auto serverInstance = reinterpret_cast<RestServer *>(req->user_ctx);

    uint8_t mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, mac);
    snprintf(serverInstance->scratchBuffer, RestServer::ScratchBufferSize,
             "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    std::string_view authMode = Wireless::getAuthModeAsString(Wireless::staInfos.authmode);

    cJSON *jsonRoot = cJSON_CreateObject();
    cJSON_AddStringToObject(jsonRoot, "ssid",
                            reinterpret_cast<const char *>(Wireless::staInfos.ssid));

    snprintf(serverInstance->scratchBuffer, RestServer::ScratchBufferSize, IPSTR,
             IP2STR(&Wireless::ipAdress));
    cJSON_AddStringToObject(jsonRoot, "ipAdress", serverInstance->scratchBuffer);

    snprintf(serverInstance->scratchBuffer, RestServer::ScratchBufferSize,
             "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    cJSON_AddStringToObject(jsonRoot, "macAdress", serverInstance->scratchBuffer);

    cJSON_AddNumberToObject(jsonRoot, "channel", Wireless::staInfos.channel);
    cJSON_AddStringToObject(jsonRoot, "authMode", authMode.data());

    std::string_view jsonData = cJSON_Print(jsonRoot);
    httpd_resp_sendstr(req, jsonData.data());
    cJSON_Delete(jsonRoot);
    return ESP_OK;
}
