#include "RenderTask.hpp"
#include "Timebase.hpp"
#include "helpers/freertos.hpp"

#include "esp_log.h"

void RenderTask::taskMain(void *)
{
    ledControl.init();

    bool blinkState = true;
    auto lastWakeTime = xTaskGetTickCount();

    while (true)
    {
        clearDisplayRam();
        renderTitleBar(blinkState);
        renderVehicles(blinkState);
        renderer.render();

        blinkState = !blinkState;

        constexpr auto TaskDelay = 1.0_s;
        xTaskDelayUntil(&lastWakeTime, toOsTicks(TaskDelay));
    }
}

//--------------------------------------------------------------------------------------------------
void RenderTask::clearDisplayRam()
{
    renderer.clearAll();
}

//--------------------------------------------------------------------------------------------------
void RenderTask::renderTitleBar(bool showDoublePoint)
{
    renderer.print({0, 0}, dfi.getStationName().data());

    std::tm *localTime = Timebase::getLocaltime(Timebase::getCurrentUTC());

    const char *clockFormat = showDoublePoint ? "%02d:%02d" : "%02d %02d";

    snprintf(printBuffer, PrintBufferSize, clockFormat, localTime->tm_hour, localTime->tm_min);
    renderer.print({LedControl::Columns, 0}, printBuffer, Renderer::Alignment::Right);
}

//--------------------------------------------------------------------------------------------------
void RenderTask::renderVehicles(bool showCurrentVehicle)
{
    size_t pageCounter = 1;

    for (auto &vehicle : dfi.getVehicles())
    {
        if (vehicle.lineNumber == "" && vehicle.directionName == "")
            continue;

        if (pageCounter == LedControl::Strips)
            break;

        size_t lineNumberPixelWidth = 0;
        const bool IsTrainAtStation = vehicle.arrivalInMinutes == 0;

        try
        {
            // print line number
            snprintf(printBuffer, PrintBufferSize, "%s ", vehicle.lineNumber.substr(6).data());
            lineNumberPixelWidth = renderer.print({0, pageCounter}, printBuffer);
        }
        catch (...)
        {
            ESP_LOGE(PrintTag, "line number is not valid, reject it: %s",
                     vehicle.lineNumber.data());
            continue;
        }

        // print line direction, blinking when train is at station
        if (!IsTrainAtStation || showCurrentVehicle)
            renderer.print({lineNumberPixelWidth, pageCounter}, vehicle.directionName.data());

        if (!IsTrainAtStation)
        {
            // print arrival time in minutes
            snprintf(printBuffer, PrintBufferSize, "%dmin", vehicle.arrivalInMinutes);
            renderer.print({LedControl::Columns, pageCounter}, printBuffer,
                           Renderer::Alignment::Right);
        }

        pageCounter++;
    }
}