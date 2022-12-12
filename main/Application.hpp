#pragma once

#include "dfi/Dfi.hpp"
// #include "StatusLed.hpp"
#include "led/LedControl.hpp"
#include "led/RenderTask.hpp"
#include "light_sensor/LightSensor.hpp"
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

    Dfi dfi{isConnected};
    LedControl ledControl{};
    RenderTask renderTask{dfi, ledControl};
    LightSensor lightSensor{ledControl};

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