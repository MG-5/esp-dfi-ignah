#include "RestServer.hpp"
#include "RestApiHandlers.hpp"

#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_system.h"
#include "lwip/apps/netbiosns.h"
#include "mdns.h"

#include <memory>

//--------------------------------------------------------------------------------------------------
void RestServer::initServer()
{
    initialiseMdns();
    netbiosns_init();
    netbiosns_set_name(MdnsHostName);

    // ESP_ERROR_CHECK(initFileSystem());
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
    config.max_uri_handlers = 32;

    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(PrintTag, "Starting REST server");
    ESP_ERROR_CHECK(httpd_start(&server, &config));

    httpd_uri_t systemInfoGetUri = {.uri = "/system/info",
                                    .method = HTTP_GET,
                                    .handler = RestApiHandlers::systemInfoGetHandler,
                                    .user_ctx = this};
    httpd_register_uri_handler(server, &systemInfoGetUri);

    httpd_uri_t systemClock = {.uri = "/system/clock",
                               .method = HTTP_GET,
                               .handler = RestApiHandlers::systemClockGetHandler,
                               .user_ctx = this};
    httpd_register_uri_handler(server, &systemClock);

    httpd_uri_t wifiStationGetUri = {.uri = "/wifi/station",
                                     .method = HTTP_GET,
                                     .handler = RestApiHandlers::wifiStationGetHandler,
                                     .user_ctx = this};
    httpd_register_uri_handler(server, &wifiStationGetUri);

    httpd_uri_t modeSetUri = {.uri = "/mode",
                              .method = HTTP_PUT,
                              .handler = RestApiHandlers::modeSetHandler,
                              .user_ctx = this};
    httpd_register_uri_handler(server, &modeSetUri);

    httpd_uri_t modeGetUri = {.uri = "/mode",
                              .method = HTTP_GET,
                              .handler = RestApiHandlers::modeGetHandler,
                              .user_ctx = this};
    httpd_register_uri_handler(server, &modeGetUri);

    httpd_uri_t freeTextSetUri = {.uri = "/freetext",
                                  .method = HTTP_PUT,
                                  .handler = RestApiHandlers::freeTextSetHandler,
                                  .user_ctx = this};
    httpd_register_uri_handler(server, &freeTextSetUri);

    httpd_uri_t freeTextGetUri = {.uri = "/freetext",
                                  .method = HTTP_GET,
                                  .handler = RestApiHandlers::freeTextGetHandler,
                                  .user_ctx = this};
    httpd_register_uri_handler(server, &freeTextGetUri);

    httpd_uri_t runningTextSetUri = {.uri = "/runningtext",
                                     .method = HTTP_PUT,
                                     .handler = RestApiHandlers::runningTextSetHandler,
                                     .user_ctx = this};
    httpd_register_uri_handler(server, &runningTextSetUri);

    httpd_uri_t runningTextGetUri = {.uri = "/runningtext",
                                     .method = HTTP_GET,
                                     .handler = RestApiHandlers::runningTextGetHandler,
                                     .user_ctx = this};
    httpd_register_uri_handler(server, &runningTextGetUri);

    httpd_uri_t additionalVehiclesSetUri = {.uri = "/additionalvehicles",
                                            .method = HTTP_PUT,
                                            .handler =
                                                RestApiHandlers::additionalVehiclesSetHandler,
                                            .user_ctx = this};
    httpd_register_uri_handler(server, &additionalVehiclesSetUri);

    httpd_uri_t additionalVehiclesGetUri = {.uri = "/additionalvehicles",
                                            .method = HTTP_GET,
                                            .handler =
                                                RestApiHandlers::additionalVehiclesGetHandler,
                                            .user_ctx = this};
    httpd_register_uri_handler(server, &additionalVehiclesGetUri);

    httpd_uri_t stationGetUri = {.uri = "/dfi/station",
                                 .method = HTTP_GET,
                                 .handler = RestApiHandlers::stationGetHandler,
                                 .user_ctx = this};
    httpd_register_uri_handler(server, &stationGetUri);

    httpd_uri_t stationSetUri = {.uri = "/dfi/station",
                                 .method = HTTP_PUT,
                                 .handler = RestApiHandlers::stationSetHandler,
                                 .user_ctx = this};
    httpd_register_uri_handler(server, &stationSetUri);

    httpd_uri_t lightSensorGetUri = {.uri = "/lightsensor",
                                     .method = HTTP_GET,
                                     .handler = RestApiHandlers::lightSensorGetHandler,
                                     .user_ctx = this};
    httpd_register_uri_handler(server, &lightSensorGetUri);

    httpd_uri_t lightSensorSetUri = {.uri = "/lightsensor",
                                     .method = HTTP_PUT,
                                     .handler = RestApiHandlers::lightSensorSetHandler,
                                     .user_ctx = this};
    httpd_register_uri_handler(server, &lightSensorSetUri);

    // URI handler for getting web server files
    httpd_uri_t commonGetUri = {.uri = "/*", //
                                .method = HTTP_GET,
                                .handler = RestApiHandlers::commonGetHandler,
                                .user_ctx = this};
    httpd_register_uri_handler(server, &commonGetUri);

    httpd_uri_t optionsUri = {.uri = "/*", //
                              .method = HTTP_OPTIONS,
                              .handler =
                                  [](httpd_req_t *req)
                              {
                                  ESP_LOGI("RestApiHandlers", "HTTP_OPTIONS");

                                  RestApiHandlers::addCorsHeaders(req);
                                  httpd_resp_set_status(req, HTTPD_200);
                                  httpd_resp_send(req, NULL, 0);

                                  return ESP_OK;
                              },
                              .user_ctx = this};
    httpd_register_uri_handler(server, &optionsUri);

    return ESP_OK;
}

//--------------------------------------------------------------------------------------------------
esp_err_t RestServer::setContentTypeFromFile(httpd_req_t *req, std::string filePath)
{
    std::string_view type = "text/plain";

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

    ESP_LOGD(PrintTag, "content type: %s", type.data());

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