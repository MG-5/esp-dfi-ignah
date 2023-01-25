#include "RenderTask.hpp"
#include "Timebase.hpp"
#include "helpers/freertos.hpp"

#include "esp_app_desc.h"
#include "esp_log.h"

void RenderTask::taskMain(void *)
{
    ledControl.init();

    bool blinkState = true;
    auto lastWakeTime = xTaskGetTickCount();
    constexpr auto TaskNormalDelay = 1.0_s;

    uint32_t prevTicks = lastWakeTime;
    clearDisplayRam();

    while (true)
    {
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
            renderTitleBar(blinkState);
            renderVehicles(blinkState);
            break;

        case State::ShowVehiclesWithRunningText:
        {
            if (ticksToTime(lastWakeTime - prevTicks) >= TaskNormalDelay)
            {
                // due higher refresh rate causes by running text, we ensures the blinking state is
                // still every one second and refresh content also only every second to reduce load
                prevTicks = lastWakeTime;
                blinkState = !blinkState;

                clearDisplayRam();
                renderTitleBar(blinkState);
                renderVehicles(blinkState);
            }

            renderRunningText();

            renderer.render();
            xTaskDelayUntil(&lastWakeTime,
                            toOsTicks((runningTextSpeed == 0.0_Hz) ? (1_ / TaskNormalDelay)
                                                                   : runningTextSpeed));

            continue;
        }

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

        xTaskDelayUntil(&lastWakeTime, toOsTicks(TaskNormalDelay));
        clearDisplayRam();
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
    const esp_app_desc_t *appDesc = esp_app_get_description();

    renderer.print({0, 0}, appDesc->project_name);
    snprintf(printBuffer, PrintBufferSize, "%s (%s)", appDesc->date, appDesc->version);
    renderer.print({0, 1}, printBuffer);
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

//--------------------------------------------------------------------------------------------------
void RenderTask::renderRunningText()
{
    constexpr auto LastPage = LedControl::Strips - 1;
    renderer.clearPage(LastPage);

    if (runningTextSpeed == 0.0_Hz)
    {
        // no speed -> pin text to left
        renderer.print({0, LastPage}, runningText.c_str());
        return;
    }

    renderer.print({runningTextPosition, LastPage}, runningText.c_str(),
                   Renderer::Alignment::Right);

    if (runningTextPosition < LedControl::Columns / 2)
    {
        // after the half strip is on the left side, move in the text second time from right side

        size_t secondPosition =
            (LedControl::Columns / 2) + runningTextPosition + runningTextWidthInPixels;
        renderer.print({secondPosition, LastPage}, runningText.c_str(), Renderer::Alignment::Right);
    }

    if (--runningTextPosition == 0)
        runningTextPosition = (LedControl::Columns / 2) + runningTextWidthInPixels;
}
