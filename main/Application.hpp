#pragma once

#include "dfi/Dfi.hpp"
// #include "StatusLed.hpp"
#include "led/LedControl.hpp"
#include "led/RenderTask.hpp"
#include "light_sensor/LightSensor.hpp"
#include "nvm/NonVolatileMemory.hpp"
#include "rest/RestServer.hpp"
#include "wifi/Wireless.hpp"

class Application
{
public:
    static constexpr auto PrintTag = "[Application]";

    Application()
    {
        timeoutTimer = xTimerCreate("timeoutTimer", toOsTicks(10.0_s), pdFALSE, nullptr, onTimeout);
    }

    void run();

    static Application &getApplicationInstance();

    static void onTimeout(TimerHandle_t);

private:
    bool isConnected = false;

    Wireless wireless{isConnected};
    // StatusLed statusLed{isConnected, pulseDetected};

    NonVolatileMemory nvm{};
    Settings settings{nvm};
    Dfi dfi{isConnected, settings};
    LedControl ledControl{};
    RenderTask renderTask{dfi, ledControl};
    LightSensor lightSensor{ledControl, settings};
    RestServer restServer{renderTask, dfi, settings};

    inline static TimerHandle_t timeoutTimer = nullptr;

    static void stopTimer()
    {
        xTimerStop(timeoutTimer, 0);
    }

    static void resetTimer()
    {
        xTimerReset(timeoutTimer, 0);
    }
};