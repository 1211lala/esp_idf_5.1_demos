#include "datafile.h"
#include "ledc.h"
#include "gpio.h"
#include "wifi_sta.h"

void tcp_client_task(void *arg);
void led_task(void *arg);
TaskHandle_t ledc_handle;
TaskHandle_t motor_handle;
bool is_connect_server = false;

struct WIFI_PARAM sta = {
    .esp_event_cb = sta_event_handler,
    .retry_num = 0,
    .info.ssid = "esp32-car-ap",
    .info.password = "1234567890",
    .is_connect = false,
};

void app_main()
{
    gpio_general_init(LED, GPIO_MODE_INPUT_OUTPUT, true);
    if (pdPASS != xTaskCreate(led_task, "led_task", 1024 * 4, NULL, 3, NULL))
    {
    }
    sta.event_group = xEventGroupCreate();
    wifi_sta_scan_connect(&sta);
    if (!sta.is_connect)
    {
        esp_restart();
    }
    ESP_LOGI("app_main", "sta is connect!!!");

    if (pdPASS != xTaskCreate(task_ledc, "task_ledc", 1024 * 4, NULL, 4, &ledc_handle))
    {
    }
    if (pdPASS != xTaskCreate(tcp_client_task, "tcp_client_task", 1024 * 4, NULL, 5, NULL))
    {
    }
}

void tcp_client_task(void *arg)
{
    const char *serverIP = "192.168.20.10";
    const int serverPort = 8000;
connect_server:
    while (!sta.is_connect)
    {
        ESP_LOGE("task_tcp_client", "等待wifi连接");
        vTaskDelay(500 / portTICK);
    }
    int tcpfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    struct sockaddr_in serverAddress = {};
    serverAddress.sin_family = AF_INET;
    inet_pton(AF_INET, serverIP, &serverAddress.sin_addr.s_addr);
    serverAddress.sin_port = htons(serverPort);

    if (0 != connect(tcpfd, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_in)))
    {
        ESP_LOGE("task_tcp_client", "tcp client connect failed");
        close(tcpfd);
        goto connect_server;
    }
    is_connect_server = true;
    ESP_LOGI("task_tcp_client", "tcp server connect success!!!");
    uint8_t r_buf[10];
    while (1)
    {
        ssize_t ret = recv(tcpfd, r_buf, 6, 0);
        if (ret <= 0)
        {
            ESP_LOGW("task_tcp_client", "断开连接");
            is_connect_server = false;
            close(tcpfd);
            goto connect_server;
        }
        MotorA.speed = r_buf[2] - 0x80;
        MotorA.angle = r_buf[3] - 0x80;
    }
    close(tcpfd);
}

void led_task(void *arg)
{
    while (1)
    {
        if (!sta.is_connect)
        {
            gpio_tiggle(LED);
            vTaskDelay(200 / portTICK);
        }
        else if (sta.is_connect && is_connect_server == false)
        {
            gpio_tiggle(LED);
            vTaskDelay(1000 / portTICK);
        }
        else if (sta.is_connect && is_connect_server)
        {
            gpio_set_level(LED, true);
            vTaskDelay(700 / portTICK);
        }
    }
}
