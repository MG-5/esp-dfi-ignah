#include "RestApiHandlers.hpp"
#include "RestServer.hpp"
#include "Timebase.hpp"
#include "wifi/Wireless.hpp"

#include "cJSON.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_wifi.h"
#include <fcntl.h>

#include <algorithm>

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

//--------------------------------------------------------------------------------------------------
esp_err_t RestApiHandlers::modeSetHandler(httpd_req_t *req)
{
    ESP_LOGI(PrintTag, "modeSetHandler");
    auto serverInstance = reinterpret_cast<RestServer *>(req->user_ctx);

    auto contentLength = req->content_len;

    if (contentLength >= serverInstance->ScratchBufferSize)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }

    size_t currrentLength = 0;
    int received = 0;

    while (currrentLength < contentLength)
    {
        received =
            httpd_req_recv(req, serverInstance->scratchBuffer + currrentLength, contentLength);
        if (received <= 0)
        {
            // Respond with 500 Internal Server Error
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR,
                                "Failed to post control value");
            return ESP_FAIL;
        }
        currrentLength += received;
    }

    ESP_LOGI(PrintTag, "set mode to %s", serverInstance->scratchBuffer);

    int modeNumber = atoi(serverInstance->scratchBuffer);

    if (modeNumber != std::clamp(modeNumber, 0, 2))
    {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "mode does not exist");
        return ESP_FAIL;
    }

    // skip first two entries
    RenderTask::State state = static_cast<RenderTask::State>(modeNumber + 2);
    serverInstance->renderTask.setState(state);

    httpd_resp_set_status(req, HTTPD_200);
    httpd_resp_send(req, NULL, 0);

    return ESP_OK;
}

//--------------------------------------------------------------------------------------------------
esp_err_t RestApiHandlers::modeGetHandler(httpd_req_t *req)
{
    ESP_LOGI(PrintTag, "modeGetHandler");
    auto serverInstance = reinterpret_cast<RestServer *>(req->user_ctx);

    // skip first two entries
    const size_t ModeNumber = static_cast<size_t>(serverInstance->renderTask.getState()) - 2;

    httpd_resp_sendstr(req, std::to_string(ModeNumber).data());

    return ESP_OK;
}

/*
static esp_err_t light_brightness_post_handler(httpd_req_t *req)
{
    int total_len = req->content_len;
    int cur_len = 0;
    char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
    int received = 0;
    if (total_len >= SCRATCH_BUFSIZE) {
        // Respond with 500 Internal Server Error
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            // Respond with 500 Internal Server Error
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control
value"); return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    cJSON *root = cJSON_Parse(buf);
    int red = cJSON_GetObjectItem(root, "red")->valueint;
    int green = cJSON_GetObjectItem(root, "green")->valueint;
    int blue = cJSON_GetObjectItem(root, "blue")->valueint;
    ESP_LOGI(REST_TAG, "Light control: red = %d, green = %d, blue = %d", red, green, blue);
    cJSON_Delete(root);
    httpd_resp_sendstr(req, "Post control value successfully");
    return ESP_OK;
}
*/