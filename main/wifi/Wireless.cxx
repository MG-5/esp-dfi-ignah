#include "Wireless.hpp"
#include "helpers/freertos.hpp"
#include "loginData.hpp"
#include "sync.hpp"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"

#include <cstring>

using namespace util::wrappers;

//--------------------------------------------------------------------------------------------------
void Wireless::taskMain(void *)
{
    startNvs();
    init();
    configureStation();
    configureSoftAp();
    startWifi();

    while (true)
    {
        if (isConnected)
        {
            sync::waitForAll(sync::ConnectionFailed);
            isConnected = false;
        }
        else
        {
            sync::waitForAll(sync::ConnectedToWifi);
            isConnected = true;
        }
    }
}

//--------------------------------------------------------------------------------------------------
void Wireless::startNvs()
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

//--------------------------------------------------------------------------------------------------
void Wireless::init()
{
    // init TCP stack
    ESP_ERROR_CHECK(esp_netif_init());

    // create event loop to get actions like connected/fail to connect
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                                        &Wireless::eventHandler, nullptr, nullptr));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                                        &Wireless::eventHandler, nullptr, nullptr));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
}
//--------------------------------------------------------------------------------------------------
void Wireless::configureStation()
{
    wifi_sta_config_t staConfig{};
    std::memcpy(staConfig.ssid, StaSsid.data(), StaSsid.length());
    std::memcpy(staConfig.password, StaPassword.data(), StaPassword.length());
    staConfig.threshold.authmode = WIFI_AUTH_WPA2_PSK; // don't accept auths below this threshold
    staConfig.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;

    wifi_config_t wifiConfig;
    wifiConfig.sta = staConfig;
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifiConfig));
}

//--------------------------------------------------------------------------------------------------
void Wireless::configureSoftAp()
{
    wifi_ap_config_t apConfig{};
    std::memcpy(apConfig.ssid, ApSsid.data(), ApSsid.length());
    std::memcpy(apConfig.password, ApPassword.data(), ApPassword.length());
    apConfig.authmode = WIFI_AUTH_WPA2_PSK;
    apConfig.max_connection = 4;

    wifi_config_t wifiConfig;
    wifiConfig.ap = apConfig;
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifiConfig));
}

//--------------------------------------------------------------------------------------------------
void Wireless::startWifi()
{
    ESP_ERROR_CHECK(esp_wifi_start());
}

//--------------------------------------------------------------------------------------------------
void Wireless::eventHandler(void *arg, esp_event_base_t eventBase, int32_t eventId, void *eventData)
{
    static constexpr auto PrintTag = "[Wireless::eventHandler]";

    if (eventBase == WIFI_EVENT)
    {
        switch (eventId)
        {
        case WIFI_EVENT_STA_START:
            ESP_LOGI(PrintTag, "Wifi driver started");
            ESP_ERROR_CHECK(esp_wifi_connect());
            break;

        case WIFI_EVENT_AP_STACONNECTED:
        {
            wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)eventData;
            ESP_LOGI(PrintTag, "station " MACSTR " join, AID=%d", MAC2STR(event->mac), event->aid);
        }
        break;

        case WIFI_EVENT_AP_STADISCONNECTED:
        {
            wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)eventData;
            ESP_LOGI(PrintTag, "station " MACSTR " leave, AID=%d", MAC2STR(event->mac), event->aid);
        }
        break;

        case WIFI_EVENT_STA_DISCONNECTED:
        {
            wifi_event_sta_disconnected_t *disconnected =
                reinterpret_cast<wifi_event_sta_disconnected_t *>(eventData);
            ESP_LOGE(PrintTag, "Wifi disconnect. Reason : %d", disconnected->reason);

            sync::clearEvents(sync::ConnectedToWifi);
            sync::signal(sync::ConnectionFailed);

            if (++reconnectionCounter >= ReconnectionCounterThreshould)
            {
                reconnectionCounter = 0;
                ESP_LOGW(PrintTag, "Restart wifi driver.");
                ESP_ERROR_CHECK(esp_wifi_stop());
                vTaskDelay(toOsTicks(RetryDelay));
                ESP_ERROR_CHECK(esp_wifi_start());
                break;
            }

            vTaskDelay(toOsTicks(RetryDelay));
            ESP_LOGW(PrintTag, "try to reconnnect");
            ESP_ERROR_CHECK(esp_wifi_connect());
        }
        break;

        default:
            ESP_LOGD(PrintTag, "event: %d", eventId);
            break;
        }
    }
    else if (eventBase == IP_EVENT && eventId == IP_EVENT_STA_GOT_IP)
    {
        reconnectionCounter = 0;
        ip_event_got_ip_t *event = static_cast<ip_event_got_ip_t *>(eventData);
        ESP_LOGI(PrintTag, "Established a wifi connection to %s", StaSsid.data());
        ESP_LOGI(PrintTag, "IP address: " IPSTR, IP2STR(&event->ip_info.ip));

        sync::clearEvents(sync::ConnectionFailed);
        sync::signal(sync::ConnectedToWifi);
    }
}