
#include "main.h"
TaskHandle_t wifi_handle;

void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_AP_START)
    {
        ESP_LOGI("wifi_event_handler", "AP started");
    }
    else if (event_id == WIFI_EVENT_AP_STOP)
    {
        ESP_LOGE("wifi_event_handler", "AP stopped");
    }
    else if (event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        ESP_LOGI("wifi_event_handler", "have a new device connect");
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        ESP_LOGE("wifi_event_handler", "have a device disconnect");
    }
}

struct WIFI_PARAM wp = {
    .esp_event_cb = wifi_event_handler,
    .info.ssid = "esp32-ap-test-wifi",
    .info.password = "1234567890",
    .info.ip = "192.168.20.10",
    .info.gateway = "192.168.20.1",
    .info.subnet = "255.255.255.0",
    
};

void task_wifi(void *arg)
{
    wifi_ap_start(&wp);
    while (1)
    {
        vTaskDelay(1000 / portTICK);
    }
}

void app_main()
{
    xTaskCreate(task_wifi, "task_wifi", 1024 * 4, NULL, 5, &wifi_handle);
    vTaskDelete(NULL);
}
