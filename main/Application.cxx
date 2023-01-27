#include "Application.hpp"

#include "Timebase.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "sync.hpp"

#include <memory>

using namespace util::wrappers;

// called by ESP-IDF
extern "C" void app_main(void) // NOLINT
{
    auto previousHeapFreeSpace = esp_get_free_heap_size();
    auto &app = Application::getApplicationInstance();
    auto currentHeapFreeSpace = esp_get_free_heap_size();

    ESP_LOGI(Application::PrintTag, "Moin");
    ESP_LOGI(Application::PrintTag, "Free memory: %d bytes", currentHeapFreeSpace);
    ESP_LOGI(Application::PrintTag, "Application consumes %d bytes on heap",
             (previousHeapFreeSpace - currentHeapFreeSpace));

    vTaskDelay(toOsTicks(100.0_ms));

    app.run();
}

//--------------------------------------------------------------------------------------------------
void Application::run()
{
    Task::applicationIsReadyStartAllTasks();

    sync::waitForAll(sync::ConnectedToWifi);
    renderTask.setState(RenderTask::State::WaitForTimesyncronization);
    Timebase::initTimeSychronization();

    resetTimer();
    sync::waitForAll(sync::TimeIsSynchronized);
    renderTask.setState(RenderTask::State::ShowVehicles);
    restServer.initServer();
    stopTimer();

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

//--------------------------------------------------------------------------------------------------
void Application::onTimeout(TimerHandle_t)
{
    ESP_LOGW(PrintTag, "No response from SNTP. Restart it.");
    Timebase::initTimeSychronization();
    resetTimer();
}