#pragma once

#include "sync.hpp"

#include "nvs.h"
#include "nvs_flash.h"
#include "nvs_handle.hpp"
#include <esp_log.h>

class NonVolatileMemory
{
public:
    static constexpr auto PrintTag = "[NonVolatileMemory]";

    NonVolatileMemory()
    {
        initNVS();
        util::wrappers::sync::signal(util::wrappers::sync::NvmInitialized);
    };

    /// multithread-safe read from NVS
    template <typename T>
    void read(std::string_view key, T &value)
    {
        esp_err_t err = handle->get_item(key.data(), value);

        switch (err)
        {
        case ESP_OK:
            ESP_LOGI(PrintTag, "Load \"%s\" from NVS.", key.data());
            break;

        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGE(PrintTag,
                     "The value \"%s\" is not initialized yet! Set it to internal default value.",
                     key.data());
            write(key, value); // value contains default value
            break;

        default:
            ESP_LOGE(PrintTag, "Error (%s) while reading %s from NVS!\n", esp_err_to_name(err),
                     key.data());
            break;
        }
    }

    /// multithread-safe write to NVS
    template <typename T>
    void write(std::string_view key, T &value)
    {
        esp_err_t err = handle->set_item(key.data(), value);

        if (err != ESP_OK)
            ESP_LOGE(PrintTag, "Error (%s) while writing to %s!\n", esp_err_to_name(err),
                     key.data());
    }

    template <typename T>
    void updateValue(std::string_view key, T &value)
    {
        write(key, value);
        // commitValues(); // not needed at IDF 5.1.1
    }

    /// at version IDF 5.1.1 this function is not needed because NVS is automatically commited after
    /// each write
    void commitValues()
    {
        ESP_LOGI(PrintTag, "Committing values to NVS ... ");

        esp_err_t err = handle->commit();

        if (err != ESP_OK)
            ESP_LOGE(PrintTag, "Error (%s) while committing values to NVS!\n",
                     esp_err_to_name(err));
    }

private:
    std::unique_ptr<nvs::NVSHandle> handle = nullptr;

    void initNVS()
    {
        esp_err_t err = nvs_flash_init();

        if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
            // NVS partition was truncated and needs to be erased
            // Retry nvs_flash_init
            ESP_LOGW(PrintTag, "Erasing NVS flash ... ");
            ESP_ERROR_CHECK(nvs_flash_erase());
            err = nvs_flash_init();
        }
        ESP_ERROR_CHECK(err);

        handle = nvs::open_nvs_handle("storage", NVS_READWRITE, &err);

        ESP_ERROR_CHECK(err);
        ESP_LOGI(PrintTag, "Inited NVS successfully");
    }
};