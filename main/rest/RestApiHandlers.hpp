#pragma once

#include "esp_http_server.h"

class RestApiHandlers
{
public:
    static constexpr auto PrintTag = "[RestApiHandlers]";

    RestApiHandlers() = delete;

    static esp_err_t commonGetHandler(httpd_req_t *req);

    static esp_err_t systemInfoGetHandler(httpd_req_t *req);
    static esp_err_t systemClockGetHandler(httpd_req_t *req);

    static esp_err_t wifiStationGetHandler(httpd_req_t *req);

    static esp_err_t modeSetHandler(httpd_req_t *req);
    static esp_err_t modeGetHandler(httpd_req_t *req);

    static esp_err_t freeTextSetHandler(httpd_req_t *req);
    static esp_err_t runningTextSetHandler(httpd_req_t *req);

    static esp_err_t additionalVehiclesSetHandler(httpd_req_t *req);
    static esp_err_t additionalVehiclesGetHandler(httpd_req_t *req);

    static esp_err_t loadContentToBuffer(httpd_req_t *req);
};