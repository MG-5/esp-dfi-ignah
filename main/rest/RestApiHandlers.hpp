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

    static esp_err_t loadContentToBuffer(httpd_req_t *req);
};
