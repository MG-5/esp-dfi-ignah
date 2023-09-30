#pragma once

#include "helpers/freertos.hpp"
#include "sync.hpp"
#include "wrappers/Task.hpp"

#include "nvs.h"
#include "nvs_flash.h"
#include "nvs_handle.hpp"
#include <esp_log.h>

using namespace util::wrappers;

class NonVolatileMemory : public TaskWithMemberFunctionBase
{
public:
    static constexpr auto PrintTag = "NonVolatileMemory";

    NonVolatileMemory()
        : TaskWithMemberFunctionBase("nvmTask", 2048, osPriorityAboveNormal2) //
          {};

    template <typename T>
    void read(std::string_view key, T &value)
    {
        esp_err_t err = handle->get_item(key.data(), value);

        switch (err)
        {
        case ESP_OK:
            break;

        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGE(PrintTag, "The value \"%s\" is not initialized yet!", key.data());
            break;

        default:
            ESP_LOGE(PrintTag, "Error (%s) while reading %s from NVS!\n", esp_err_to_name(err),
                     key.data());
            break;
        }
    }

    template <typename T>
    void write(std::string_view key, T &value)
    {
        esp_err_t err = handle->set_item(key.data(), value);

        if (err != ESP_OK)
            ESP_LOGE(PrintTag, "Error (%s) while writing %s to %s!\n", esp_err_to_name(err), value,
                     key.data());
    }

    void commitValues()
    {
        ESP_LOGI(PrintTag, "Committing values to NVS ... ");

        esp_err_t err = handle->commit();

        if (err != ESP_OK)
            ESP_LOGE(PrintTag, "Error (%s) while committing values to NVS!\n",
                     esp_err_to_name(err));
    }

protected:
    void taskMain(void *) override
    {
        init();
        sync::signal(sync::ConfigurationLoaded);

        vTaskSuspend(nullptr);
    }

private:
    std::unique_ptr<nvs::NVSHandle> handle = nullptr;

    void init()
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