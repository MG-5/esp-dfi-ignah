#include "RestServer.hpp"

#include "cJSON.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_system.h"
#include "lwip/apps/netbiosns.h"
#include "mdns.h"
#include <fcntl.h>

#include <memory>

//--------------------------------------------------------------------------------------------------
void RestServer::initServer()
{
    initialiseMdns();
    netbiosns_init();
    netbiosns_set_name(MdnsHostName);

    ESP_ERROR_CHECK(startServer(WebMountPoint));
}

//--------------------------------------------------------------------------------------------------
void RestServer::initialiseMdns()
{
    ESP_ERROR_CHECK(mdns_init());
    ESP_ERROR_CHECK(mdns_hostname_set(MdnsHostName));
    ESP_ERROR_CHECK(mdns_instance_name_set(FriendlyName));

    mdns_txt_item_t serviceTxtData[] = {{"board", "esp32"}, {"path", "/"}};

    ESP_ERROR_CHECK(mdns_service_add("ESP32-WebServer", "_http", "_tcp", 80, serviceTxtData,
                                     sizeof(serviceTxtData) / sizeof(mdns_txt_item_t)));
}

//--------------------------------------------------------------------------------------------------
esp_err_t RestServer::initFileSystem()
{
    esp_vfs_spiffs_conf_t conf = {.base_path = WebMountPoint,
                                  .partition_label = NULL,
                                  .max_files = 5,
                                  .format_if_mount_failed = false};

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
            ESP_LOGE(PrintTag, "Failed to mount or format filesystem");

        else if (ret == ESP_ERR_NOT_FOUND)
            ESP_LOGE(PrintTag, "Failed to find SPIFFS partition");

        else
            ESP_LOGE(PrintTag, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));

        return ESP_FAIL;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK)
        ESP_LOGE(PrintTag, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));

    else
        ESP_LOGI(PrintTag, "Partition size: total: %d, used: %d", total, used);

    return ESP_OK;
}

//--------------------------------------------------------------------------------------------------
esp_err_t RestServer::startServer(std::string newBasePath)
{
    if (newBasePath.empty())
    {
        ESP_LOGE(PrintTag, "base path should not be empty!");
        return ESP_FAIL;
    }

    basePath = newBasePath;

    httpd_handle_t server = nullptr;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(PrintTag, "Starting REST server");
    ESP_ERROR_CHECK(httpd_start(&server, &config));

    httpd_uri_t systemInfoGetUri = {.uri = "/api/v1/system/info",
                                    .method = HTTP_GET,
                                    .handler = systemInfoGetHandler,
                                    .user_ctx = this};
    httpd_register_uri_handler(server, &systemInfoGetUri);

    // URI handler for getting web server files
    httpd_uri_t commonGetUri = {.uri = "/*", //
                                .method = HTTP_GET,
                                .handler = commonGetHandler,
                                .user_ctx = this};
    httpd_register_uri_handler(server, &commonGetUri);

    return ESP_OK;
}

//--------------------------------------------------------------------------------------------------
esp_err_t RestServer::systemInfoGetHandler(httpd_req_t *req)
{
    // [[maybe_unused]] int number = reinterpret_cast<RestServer *>(req->user_ctx)->test;

    httpd_resp_set_type(req, "application/json");

    cJSON *jsonRoot = cJSON_CreateObject();
    esp_chip_info_t chipInfo;
    esp_chip_info(&chipInfo);
    cJSON_AddStringToObject(jsonRoot, "version", IDF_VER);
    cJSON_AddNumberToObject(jsonRoot, "cores", chipInfo.cores);
    std::unique_ptr<const char> sysInfo = std::make_unique<const char>(*cJSON_Print(jsonRoot));
    httpd_resp_sendstr(req, sysInfo.get());
    cJSON_Delete(jsonRoot);
    return ESP_OK;
}

//--------------------------------------------------------------------------------------------------
esp_err_t RestServer::commonGetHandler(httpd_req_t *req)
{
    std::string filePath;

    auto serverInstance = reinterpret_cast<RestServer *>(req->user_ctx);

    filePath = serverInstance->basePath;

    if (req->uri[strlen(req->uri) - 1] == '/')
        filePath += "/index.html";

    else
        filePath += req->uri;

    int fileStreamId = open(filePath.data(), O_RDONLY, 0);
    if (fileStreamId == -1)
    {
        ESP_LOGE(PrintTag, "Failed to open file: %s", filePath.data());
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }

    setContentTypeFromFile(req, filePath.data());

    size_t readBytes;
    do
    {
        // Read file in chunks into the scratch buffer
        readBytes = read(fileStreamId, serverInstance->scratchBuffer, ScratchBufferSize);
        if (readBytes == -1)
            ESP_LOGE(PrintTag, "Failed to read file: %s", filePath.data());

        else if (readBytes > 0)
        {
            // Send the buffer contents as HTTP response chunk
            if (httpd_resp_send_chunk(req, serverInstance->scratchBuffer, readBytes) != ESP_OK)
            {
                close(fileStreamId);
                ESP_LOGE(PrintTag, "Failed to send file %s", filePath.data());
                httpd_resp_sendstr_chunk(req, NULL); // Abort sending file
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                return ESP_FAIL;
            }
        }
    } while (readBytes > 0);

    close(fileStreamId);
    ESP_LOGI(PrintTag, "Sended file: %s", filePath.data());

    // Respond with an empty chunk to signal HTTP response completion
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

//--------------------------------------------------------------------------------------------------
esp_err_t RestServer::setContentTypeFromFile(httpd_req_t *req, std::string filePath)
{
    std::string type = "text/plain";

    if (checkFileExtension(filePath, ".html"))
        type = "text/html";

    else if (checkFileExtension(filePath, ".js"))
        type = "application/javascript";

    else if (checkFileExtension(filePath, ".css"))
        type = "text/css";

    else if (checkFileExtension(filePath, ".png"))
        type = "image/png";

    else if (checkFileExtension(filePath, ".ico"))
        type = "image/x-icon";

    else if (checkFileExtension(filePath, ".svg"))
        type = "text/xml";

    return httpd_resp_set_type(req, type.data());
}

//--------------------------------------------------------------------------------------------------
bool RestServer::checkFileExtension(std::string filePath, std::string extension)
{
    auto filePathLength = filePath.length();
    auto extensionLength = extension.length();

    if (filePathLength <= extensionLength)
        return false;

    try
    {
        return filePath.substr(filePathLength - extensionLength).compare(extension) == 0;
    }
    catch (...)
    {
        ESP_LOGE(PrintTag, "Cannot get substring from filepath: %s", filePath.data());
        return false;
    }
}
