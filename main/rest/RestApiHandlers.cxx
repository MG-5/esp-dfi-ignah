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
#include <sstream>

//--------------------------------------------------------------------------------------------------
esp_err_t RestApiHandlers::commonGetHandler(httpd_req_t *req)
{
    // Temporarily disable loading web server from flash
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "No webserver currently exists");
    return ESP_OK;

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
            addCorsHeaders(req);
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

                addCorsHeaders(req);
                httpd_resp_sendstr_chunk(req, nullptr); // Abort sending file
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                return ESP_FAIL;
            }
        }
    } while (readBytes > 0);

    close(fileStreamId);
    ESP_LOGI(PrintTag, "Sent file: %s", filePath.data());

    // Respond with an empty chunk to signal HTTP response completion
    addCorsHeaders(req);
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
    addCorsHeaders(req);
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
    addCorsHeaders(req);
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

    std::string_view authMode = Wireless::getAuthModeAsString(Wireless::staInfos.authmode);

    cJSON *jsonRoot = cJSON_CreateObject();
    cJSON_AddStringToObject(jsonRoot, "ssid",
                            reinterpret_cast<const char *>(Wireless::staInfos.ssid));

    snprintf(serverInstance->scratchBuffer, RestServer::ScratchBufferSize, IPSTR,
             IP2STR(&Wireless::ipAdress));
    cJSON_AddStringToObject(jsonRoot, "ipAdress", serverInstance->scratchBuffer);

    uint8_t mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, mac);
    snprintf(serverInstance->scratchBuffer, RestServer::ScratchBufferSize,
             "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    cJSON_AddStringToObject(jsonRoot, "macAdress", serverInstance->scratchBuffer);

    cJSON_AddNumberToObject(jsonRoot, "channel", Wireless::staInfos.channel);
    cJSON_AddStringToObject(jsonRoot, "authMode", authMode.data());

    std::string_view jsonData = cJSON_Print(jsonRoot);
    addCorsHeaders(req);
    httpd_resp_sendstr(req, jsonData.data());
    cJSON_Delete(jsonRoot);
    return ESP_OK;
}

//--------------------------------------------------------------------------------------------------
esp_err_t RestApiHandlers::modeSetHandler(httpd_req_t *req)
{
    ESP_LOGI(PrintTag, "modeSetHandler");
    auto serverInstance = reinterpret_cast<RestServer *>(req->user_ctx);

    if (loadContentToBuffer(req) != ESP_OK)
        return ESP_FAIL;

    ESP_LOGI(PrintTag, "set mode to %s", serverInstance->scratchBuffer);

    int modeNumber = atoi(serverInstance->scratchBuffer);

    if (modeNumber != std::clamp(modeNumber, 0, 2))
    {
        addCorsHeaders(req);
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "mode does not exist");
        return ESP_FAIL;
    }

    // skip first two entries
    RenderTask::State state = static_cast<RenderTask::State>(modeNumber + 2);
    serverInstance->renderTask.setState(state);

    addCorsHeaders(req);
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

    addCorsHeaders(req);
    httpd_resp_sendstr(req, std::to_string(ModeNumber).data());

    return ESP_OK;
}

//--------------------------------------------------------------------------------------------------
esp_err_t RestApiHandlers::freeTextSetHandler(httpd_req_t *req)
{
    constexpr auto NumberOfLines = LedControl::Strips;

    ESP_LOGI(PrintTag, "freeTextSetHandler");

    if (loadContentToBuffer(req) != ESP_OK)
        return ESP_FAIL;

    auto serverInstance = reinterpret_cast<RestServer *>(req->user_ctx);

    cJSON *root = cJSON_Parse(serverInstance->scratchBuffer);
    auto jsonLines = cJSON_GetObjectItem(root, "lines");
    if (!cJSON_IsArray(jsonLines) || cJSON_GetArraySize(jsonLines) != NumberOfLines)
    {
        addCorsHeaders(req);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                            "lines is not an array or does not contain five elements");
        cJSON_Delete(root);
        return ESP_FAIL;
    }

    std::array<std::string, NumberOfLines> lineArray{};

    for (size_t i = 0; i < NumberOfLines; i++)
        lineArray[i] = cJSON_GetArrayItem(jsonLines, i)->valuestring;

    serverInstance->renderTask.setFreeText(lineArray);

    cJSON_Delete(root);

    addCorsHeaders(req);
    httpd_resp_set_status(req, HTTPD_200);
    httpd_resp_send(req, NULL, 0);

    return ESP_OK;
}

//--------------------------------------------------------------------------------------------------
esp_err_t RestApiHandlers::freeTextGetHandler(httpd_req_t *req)
{
    ESP_LOGI(PrintTag, "freeTextGetHandler");
    auto serverInstance = reinterpret_cast<RestServer *>(req->user_ctx);

    auto jsonRoot = cJSON_CreateObject();

    auto lines = serverInstance->renderTask.getFreeText();

    auto createStringArray = [&]() -> cJSON *
    {
        cJSON *str = nullptr;
        cJSON *prev = nullptr;
        cJSON *array = nullptr;

        array = cJSON_CreateArray();

        for (int i = 0; array && (i < lines.size()); i++)
        {
            str = cJSON_CreateString(lines[i].c_str());
            if (!str)
            {
                cJSON_Delete(array);
                return nullptr;
            }

            if (!i)
                array->child = str;

            else
            {
                prev->next = str;
                str->prev = prev;
            }

            prev = str;
        }

        if (array && array->child)
            array->child->prev = str;

        return array;
    };

    auto jsonArray = createStringArray();
    cJSON_AddItemToObject(jsonRoot, "lines", jsonArray);

    std::string_view jsonData = cJSON_Print(jsonRoot);
    addCorsHeaders(req);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, jsonData.data());
    cJSON_Delete(jsonRoot);

    return ESP_OK;
}

//--------------------------------------------------------------------------------------------------
esp_err_t RestApiHandlers::runningTextSetHandler(httpd_req_t *req)
{
    ESP_LOGI(PrintTag, "runningTextSetHandler");

    if (loadContentToBuffer(req) != ESP_OK)
        return ESP_FAIL;

    auto serverInstance = reinterpret_cast<RestServer *>(req->user_ctx);

    cJSON *root = cJSON_Parse(serverInstance->scratchBuffer);

    auto text = cJSON_GetObjectItem(root, "text");
    auto speed = cJSON_GetObjectItem(root, "speed");

    if (!text || !speed || text->type != cJSON_String || speed->type != cJSON_Number)
    {
        addCorsHeaders(req);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                            "text or speed does not exist or are not right types (string for text "
                            "and int for speed)");
        cJSON_Delete(root);
        return ESP_FAIL;
    }

    std::string textString(text->valuestring);
    serverInstance->renderTask.setRunningText(textString, 1.0_Hz * (speed->valueint));

    cJSON_Delete(root);

    addCorsHeaders(req);
    httpd_resp_set_status(req, HTTPD_200);
    httpd_resp_send(req, NULL, 0);

    return ESP_OK;
}

//--------------------------------------------------------------------------------------------------
esp_err_t RestApiHandlers::runningTextGetHandler(httpd_req_t *req)
{
    ESP_LOGI(PrintTag, "runningTextGetHandler");
    auto serverInstance = reinterpret_cast<RestServer *>(req->user_ctx);

    auto jsonRoot = cJSON_CreateObject();
    auto runningTextParameters = serverInstance->renderTask.getRunningText();

    cJSON_AddStringToObject(jsonRoot, "text", runningTextParameters.first.c_str());
    cJSON_AddNumberToObject(jsonRoot, "speed", runningTextParameters.second.getMagnitude());

    std::string_view jsonData = cJSON_Print(jsonRoot);
    addCorsHeaders(req);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, jsonData.data());
    cJSON_Delete(jsonRoot);

    return ESP_OK;
}

//--------------------------------------------------------------------------------------------------
esp_err_t RestApiHandlers::additionalVehiclesSetHandler(httpd_req_t *req)
{
    ESP_LOGI(PrintTag, "additionalVehiclesSetHandler");

    if (loadContentToBuffer(req) != ESP_OK)
        return ESP_FAIL;

    auto serverInstance = reinterpret_cast<RestServer *>(req->user_ctx);
    cJSON *root = cJSON_Parse(serverInstance->scratchBuffer);
    cJSON *vehicles = cJSON_GetObjectItem(root, "vehicles");

    if (!vehicles || !cJSON_IsArray(vehicles))
    {
        addCorsHeaders(req);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                            "vehicles object does not exist or it not an array");
        cJSON_Delete(root);
        return ESP_FAIL;
    }

    Dfi::AdditionalVehicleList vehicleList{};
    auto arraySize = std::min(cJSON_GetArraySize(vehicles), Dfi::MaximumNumberOfAdditionalVehicles);

    for (size_t i = 0; i < arraySize; i++)
    {
        auto destinationObject =
            cJSON_GetObjectItem(cJSON_GetArrayItem(vehicles, i), "destination");
        auto fpTimeObject = cJSON_GetObjectItem(cJSON_GetArrayItem(vehicles, i), "departure");

        if (destinationObject == nullptr || destinationObject->type != cJSON_String ||
            fpTimeObject == nullptr || fpTimeObject->type != cJSON_String)
        {
            ESP_LOGE(PrintTag, "Array entry a index %d is not valid. This vehicle will be ignored.",
                     i);
            continue;
        }

        Dfi::LocalTransportVehicle newVehicle{};
        newVehicle.lineNumber = "Str   77";
        newVehicle.directionName = destinationObject->valuestring;

        try
        {
            newVehicle.fpTime = Time(fpTimeObject->valuestring);
        }
        catch (...)
        {
            ESP_LOGE(PrintTag,
                     "Cannot convert string \"%s\" to Time. This vehicle will be ignored.",
                     fpTimeObject->valuestring);
            continue;
        }

        vehicleList.emplace_back(newVehicle);

        ESP_LOGI(PrintTag, "%s (%.2d:%.2d)", newVehicle.directionName.c_str(),
                 newVehicle.fpTime.hour, newVehicle.fpTime.minute);
    }

    cJSON_Delete(root);

    serverInstance->dfi.setAdditionalVehicles(vehicleList);

    auto jsonRoot = cJSON_CreateObject();
    auto newArray = cJSON_AddArrayToObject(jsonRoot, "vehicles");

    for (auto &vehicle : vehicleList)
    {
        if (vehicle.lineNumber.empty())
            break;

        auto object = cJSON_CreateObject();
        cJSON_AddItemToObject(object, "destination",
                              cJSON_CreateString(vehicle.directionName.c_str()));

        std::stringstream time;
        time << vehicle.fpTime;

        cJSON_AddItemToObject(object, "departure", cJSON_CreateString(time.str().c_str()));

        cJSON_AddItemToArray(newArray, object);
    }

    std::string_view jsonData = cJSON_Print(jsonRoot);
    addCorsHeaders(req);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, jsonData.data());
    cJSON_Delete(jsonRoot);
    return ESP_OK;
}

//--------------------------------------------------------------------------------------------------
esp_err_t RestApiHandlers::additionalVehiclesGetHandler(httpd_req_t *req)
{
    ESP_LOGI(PrintTag, "additionalVehiclesGetHandler");

    auto serverInstance = reinterpret_cast<RestServer *>(req->user_ctx);
    Dfi::AdditionalVehicleList vehicleList{};
    serverInstance->dfi.getAdditionalVehicles(vehicleList);

    auto jsonRoot = cJSON_CreateObject();
    auto newArray = cJSON_AddArrayToObject(jsonRoot, "vehicles");

    for (auto it = vehicleList.begin(); it != vehicleList.end(); it++)
    {
        if (it->lineNumber.empty())
            break;

        auto object = cJSON_CreateObject();
        cJSON_AddItemToObject(object, "destination", cJSON_CreateString(it->directionName.c_str()));

        std::stringstream time;
        time << it->fpTime;

        cJSON_AddItemToObject(object, "departure", cJSON_CreateString(time.str().c_str()));

        cJSON_AddItemToArray(newArray, object);
    }

    std::string_view jsonData = cJSON_Print(jsonRoot);
    addCorsHeaders(req);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, jsonData.data());
    cJSON_Delete(jsonRoot);
    return ESP_OK;
}

//--------------------------------------------------------------------------------------------------
esp_err_t RestApiHandlers::loadContentToBuffer(httpd_req_t *req)
{
    auto serverInstance = reinterpret_cast<RestServer *>(req->user_ctx);
    auto contentLength = req->content_len;

    if (contentLength >= serverInstance->ScratchBufferSize - 1)
    {
        addCorsHeaders(req);
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
            addCorsHeaders(req);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to put/post value");
            return ESP_FAIL;
        }
        currrentLength += received;
    }

    serverInstance->scratchBuffer[contentLength] = '\0';

    return ESP_OK;
}

//--------------------------------------------------------------------------------------------------
void RestApiHandlers::addCorsHeaders(httpd_req_t *req)
{
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, PUT, OPTIONS");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");
}