#pragma once

#include "esp_event.h"
#include "helpers/freertos.hpp"
#include "wrappers/Task.hpp"

using util::wrappers::TaskWithMemberFunctionBase;

class Wireless : public TaskWithMemberFunctionBase
{
public:
    static constexpr auto RetryDelay = 3.0_s;

    Wireless(bool &isConnected)
        : TaskWithMemberFunctionBase("wirelessTask", 2048, osPriorityAboveNormal3),
          isConnected(isConnected) //
          {};

    static void eventHandler(void *arg, esp_event_base_t eventBase, int32_t eventId,
                             void *eventData);

protected:
    void taskMain(void *) override;

private:
    bool &isConnected;

    void startNvs();
    void init();
    void configureStation();
    void configureSoftAp();
    void startWifi();
};