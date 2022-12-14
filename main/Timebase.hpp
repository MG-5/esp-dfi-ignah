#pragma once

#include "esp_log.h"
#include "esp_sntp.h"

#include "sync.hpp"
#include "wrappers/Queue.hpp"

#include <chrono>

using namespace util::wrappers;

// CAUTION, this is not Y2K38-issue aware, see
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/system_time.html#bit-time-t
// for more details and possible workaround
// (at the moment of writing for fixing its needed compiling your own toolchain from stratch, why??)

/// This ensures we are getting the current UTC from time server.
/// Time synchronization will happens every
/// [CONFIG_LWIP_SNTP_UPDATE_DELAY] milliseconds (see sdkconfig)
class Timebase
{
public:
    static constexpr auto PrintTag = "[Timebase]";
    using Timestamp = std::chrono::time_point<std::chrono::system_clock>;
    using TimestampQueue = util::wrappers::Queue<Timebase::Timestamp>;

    //--------------------------------------------------------------------------------------------------
    static void initTimeSychronization()
    {
        sntp_stop();
        ESP_LOGI(PrintTag, "Initializing SNTP");
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_setservername(0, "pool.ntp.org");
        sntp_set_time_sync_notification_cb(timeSynchronizationCallback);
        sntp_init();
        ESP_LOGI(PrintTag, "Update system time every %d minutes.",
                 CONFIG_LWIP_SNTP_UPDATE_DELAY / 1000 / 60);

        // set timezone to Berlin
        setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
        tzset();
    }

    //--------------------------------------------------------------------------------------------------
    static void timeSynchronizationCallback(struct timeval *)
    {
        ESP_LOGI(PrintTag, "Time synchronization event arrived.");
        printLocaltime();
        sync::signal(sync::TimeIsSynchronized);
    }

    static void printLocaltime()
    {
        std::tm *localTime = getLocaltime(getCurrentUTC());
        ESP_LOGI(PrintTag, "local time in Berlin: %02d:%02d", localTime->tm_hour,
                 localTime->tm_min);
    }

    //--------------------------------------------------------------------------------------------------
    [[nodiscard]] static Timestamp getCurrentUTC()
    {
        return std::chrono::system_clock::now();
    }

    //--------------------------------------------------------------------------------------------------
    [[nodiscard]] static std::tm *getLocaltime(Timestamp now)
    {
        const auto CurrentTime = std::chrono::system_clock::to_time_t(now);
        return std::localtime(&CurrentTime);
    }
};
