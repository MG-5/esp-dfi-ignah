#pragma once

#include "LedControl.hpp"
#include "dfi/Dfi.hpp"
#include "display-renderer/Renderer.hpp"
#include "util/gpio.hpp"
#include "wrappers/Task.hpp"

class RenderTask : public util::wrappers::TaskWithMemberFunctionBase
{
public:
    explicit RenderTask(Dfi &dfi, LedControl &ledControl)
        : TaskWithMemberFunctionBase("renderTask", 1024, osPriorityNormal5), //
          dfi(dfi),                                                          //
          ledControl(ledControl)                                             //
          {};

    enum class State
    {
        InitializingWifi,
        WaitForTimesyncronization,
        ShowVehicles,
        ShowVehiclesWithRunningText,
        ShowFreeText
    };

    void setState(State newState)
    {
        state = newState;
    }

    State getState() const
    {
        return state;
    }

protected:
    void taskMain(void *) override;

private:
    static constexpr auto PrintTag = "[RenderTask]";
    static constexpr auto NumberOfDots = 5;

    Dfi &dfi;
    LedControl &ledControl;
    Renderer renderer{LedControl::Columns, LedControl::Strips, ledControl};

    State state = State::InitializingWifi;
    uint8_t dotCounter = 1;

    static constexpr auto PrintBufferSize = 72;
    char printBuffer[PrintBufferSize]{};

    void clearDisplayRam();
    void renderTitleBar(bool showDoublePoint);
    void renderVehicles(bool showCurrentVehicle);
    void renderProjectInfos();
    void renderConnectingToWifi();
    void renderTimesyncronization();
};
