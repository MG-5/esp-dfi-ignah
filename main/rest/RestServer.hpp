#pragma once

#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_vfs.h"

#include <string>

class RestServer
{
public:
    static constexpr auto MdnsHostName = "esp-home";
    static constexpr auto FriendlyName = "esp home web server";
    static constexpr auto WebMountPoint = "/www";
    static constexpr auto UseGzipCompression = true;

    void initServer();

private:
    static constexpr auto PrintTag = "[RestServer]";

    static constexpr size_t ScratchBufferSize = 1024 * 20;

    void initialiseMdns();
    esp_err_t initFileSystem();
    esp_err_t startServer(std::string newBasePath);

    static esp_err_t systemInfoGetHandler(httpd_req_t *req);
    static esp_err_t commonGetHandler(httpd_req_t *req);
    static esp_err_t setContentTypeFromFile(httpd_req_t *req, std::string filePath);
    static bool checkFileExtension(std::string filePath, std::string extension);

    std::string basePath = "";
    char scratchBuffer[ScratchBufferSize]{};
};
