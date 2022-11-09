#pragma once

#include "dfi/Dfi.hpp"
// #include "StatusLed.hpp"
#include "wifi/Wireless.hpp"

class Application
{
public:
    Application(){};
    void run();

    static Application &getApplicationInstance();

private:
    bool isConnected = false;

    Wireless wireless{isConnected};
    // StatusLed statusLed{isConnected, pulseDetected};
    Dfi dfi{isConnected};
};