#include "datafile.h"
#include "ledc.h"
#include "gpio.h"
#include "wifi_tcp.h"

TaskHandle_t ledc_handle;
TaskHandle_t motor_handle;

struct WIFI_PARAM ap = {
    .esp_event_cb = ap_event_handler,
    .info.ssid = "esp32-car-ap",
    .info.password = "1234567890",
    .info.ip = "192.168.20.10",
    .info.gateway = "192.168.20.1",
    .info.subnet = "255.255.255.0",
    .info.dns = "114.114.114.114",
};
void app_main()
{
    ap.event_group = xEventGroupCreate();

    wifi_ap_start(&ap);

    if (pdPASS != xEventGroupWaitBits(ap.event_group, AP_IS_START, pdFALSE, pdTRUE, portMAX_DELAY))
    {
    }
    ESP_LOGI("app_main", "ap is start!!!");
    vEventGroupDelete(ap.event_group);

    if (pdPASS != xTaskCreate(tcp_server_task, "tcp_server_task", 1024 * 4, NULL, 5, &motor_handle))
    {
    }
    if (pdPASS != xTaskCreate(task_ledc, "task_ledc", 1024 * 4, NULL, 5, &ledc_handle))
    {
    }
}
