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

    [[nodiscard]] State getState() const
    {
        return state;
    }

    void setFreeText(std::array<std::string, LedControl::Strips> &newFreeText)
    {
        freeText = newFreeText;
    }

    void setRunningText(std::string &newRunningText, size_t newSpeed)
    {
        runningText = newRunningText;
        runningTextSpeed = newSpeed;
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

    std::array<std::string, LedControl::Strips> freeText{"Zeile1", "Zeile2", "Zeile3", "Zeile4",
                                                         "Zeile5"};

    std::string runningText = "*** Lauftext ***";
    size_t runningTextSpeed = 20; // pixels per second

    static constexpr auto PrintBufferSize = 72;
    char printBuffer[PrintBufferSize]{};

    void clearDisplayRam();
    void renderTitleBar(bool showDoublePoint);
    void renderVehicles(bool showCurrentVehicle);
    void renderProjectInfos();
    void renderConnectingToWifi();
    void renderTimesyncronization();
};
