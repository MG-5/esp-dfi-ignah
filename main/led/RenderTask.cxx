#include "RenderTask.hpp"
#include "Timebase.hpp"
#include "helpers/freertos.hpp"

#include "esp_log.h"
#include "esp_ota_ops.h"

void RenderTask::taskMain(void *)
{
    ledControl.init();

    bool blinkState = true;
    auto lastWakeTime = xTaskGetTickCount();

    while (true)
    {
        clearDisplayRam();

        switch (state)
        {
        case State::InitializingWifi:
            renderProjectInfos();
            renderConnectingToWifi();
            break;

        case State::WaitForTimesyncronization:
            renderProjectInfos();
            renderTimesyncronization();
            break;

        case State::ShowVehicles:
        case State::ShowVehiclesWithRunningText:
            renderTitleBar(blinkState);
            renderVehicles(blinkState);
            break;

        case State::ShowFreeText:
            renderer.print({0, 0}, freeText[0].c_str());
            renderer.print({0, 1}, freeText[1].c_str());
            renderer.print({0, 2}, freeText[2].c_str());
            renderer.print({0, 3}, freeText[3].c_str());
            renderer.print({0, 4}, freeText[4].c_str());
            break;
        }

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

//--------------------------------------------------------------------------------------------------
void RenderTask::renderProjectInfos()
{
    const esp_app_desc_t *appDesc = esp_ota_get_app_description();

    snprintf(printBuffer, PrintBufferSize, "%s (%s)", appDesc->project_name, appDesc->version);
    renderer.print({0, 0}, printBuffer);
    renderer.print({0, 1}, appDesc->date);
    renderer.print({0, 2}, "by M. Grau und T. Wiesner");
}

//--------------------------------------------------------------------------------------------------
void RenderTask::renderConnectingToWifi()
{
    std::string textToPrint = " Mit WLAN verbinden ";

    if (dotCounter++ >= NumberOfDots)
        dotCounter = 1;

    for (size_t i = 0; i < dotCounter; i++)
        textToPrint += ".";

    renderer.print({0, LedControl::Strips - 1}, textToPrint.data());
}

//--------------------------------------------------------------------------------------------------
void RenderTask::renderTimesyncronization()
{
    std::string textToPrint = " Zeitsyncronisation ";

    if (dotCounter++ >= NumberOfDots)
        dotCounter = 1;

    for (size_t i = 0; i < dotCounter; i++)
        textToPrint += ".";

    renderer.print({0, LedControl::Strips - 1}, textToPrint.data());
}
