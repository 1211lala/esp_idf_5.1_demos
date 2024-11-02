#include "main.h"

#include "lwip/sockets.h"

TaskHandle_t wifi_handle;

struct WIFI_PARAM wp = {
    .esp_event_cb = wifi_event_handler,
    .retry_num = 0,
    .info.ssid = "艾可舒",
    .info.password = "ai666888",
    .is_connect = false,
};

void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    struct WIFI_PARAM *w = arg;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        w->retry_num = 0;
        ESP_LOGI("wifi_event_handler", "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI("wifi_event_handler", "Got GW: " IPSTR, IP2STR(&event->ip_info.gw));
        ESP_LOGI("wifi_event_handler", "Got NT: " IPSTR, IP2STR(&event->ip_info.netmask));
        w->is_connect = true;
        xEventGroupSetBits(w->event_group, WIFI_CONNECTED_BIT);
    }

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        w->is_connect = false;
        if (w->retry_num < 100)
        {
            esp_wifi_connect();
            w->retry_num++;
            ESP_LOGW("wifi_event_handler", "%s 重新开始连接AP %d/%d", w->info.ssid, w->retry_num, 100);
        }
        else
        {
            xEventGroupSetBits(w->event_group, WIFI_FAIL_BIT);
            ESP_LOGE("wifi_event_handler", "%s 连接失败", w->info.ssid);
        }
    }
}

void task_tcp_client(void *arg)
{
    wifi_sta_connect(&wp);
    const char *serverIP = "192.168.10.57";
    const int serverPort = 8000;
connect_server:
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
    ESP_LOGI("task_tcp_client", "tcp server connect success!!!");
    while (1)
    {
        uint8_t *send_buf = malloc(150);

        static uint16_t number = 10;
        uint16_t ap_count = 0;
        wifi_ap_record_t ap_info[10];
        memset(ap_info, 0, sizeof(ap_info));

        esp_wifi_scan_start(NULL, true);
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
        for (int i = 0; i < ap_count && (i < 10); i++)
        {
            sprintf((char *)send_buf, "%-20s %20d %20d\n", ap_info[i].ssid, ap_info[i].rssi, ap_info[i].primary);
            ssize_t len = send(tcpfd, send_buf, strlen((char *)send_buf), 0);
            if (len < 0)
            {
                close(tcpfd);
                free(send_buf);
                ESP_LOGE("task_tcp_client", "server disconnect");
                goto connect_server;
            }
        }
        ESP_LOGI("task_tcp_client", "tcp send");

        free(send_buf);
        vTaskDelay(5 / portTICK_PERIOD_MS); // 每5秒扫描一次
    }
    close(tcpfd);
}

void app_main()
{
    xTaskCreate(task_tcp_client, "task_tcp_client", 1024 * 4, NULL, 5, &wifi_handle);
    vTaskDelete(NULL);
}
