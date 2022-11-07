#include "Application.hpp"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include <memory>

// called by ESP-IDF
extern "C" void app_main(void) // NOLINT
{
    auto previousHeapFreeSpace = esp_get_free_heap_size();
    auto &app = Application::getApplicationInstance();
    auto currentHeapFreeSpace = esp_get_free_heap_size();

    static constexpr auto PrintTag = "[Application]";

    ESP_LOGI(PrintTag, "Moin");
    ESP_LOGI(PrintTag, "Free memory: %d bytes", currentHeapFreeSpace);
    ESP_LOGI(PrintTag, "Application consumes %d bytes on heap",
             (previousHeapFreeSpace - currentHeapFreeSpace));

    // vTaskDelay(toOsTicks(100.0_ms));

    app.run();
}

//--------------------------------------------------------------------------------------------------
void Application::run()
{
    // util::wrappers::Task::applicationIsReadyStartAllTasks();

    // sync::waitForAll(sync::ConnectedToWifi);
    //  Timebase::initTimeSychronization();

    while (true)
    {
        vTaskDelay(portMAX_DELAY);
    }
}

//--------------------------------------------------------------------------------------------------
Application &Application::getApplicationInstance()
{
    static auto app = std::make_unique<Application>();
    return *app;
}