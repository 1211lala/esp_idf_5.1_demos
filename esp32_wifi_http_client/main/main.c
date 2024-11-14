#include "main.h"

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

esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    static const char *TAG = "http_client";
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGI(TAG, "错误事件");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGI(TAG, "连接成功事件");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGI(TAG, "发送头事件");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGI(TAG, "接收头事件");
        printf("%.*s", evt->data_len, (char *)evt->data);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGI(TAG, "接收数据事件, len=%d", evt->data_len);
        if (!esp_http_client_is_chunked_response(evt->client))
        {
            printf("%.*s\r\n", evt->data_len, (char *)evt->data);

            cJSON *pJsonRoot = cJSON_Parse((char *)evt->data);
            if (pJsonRoot != NULL)
            {
                printf("这是一个JSON\r\n");
                cJSON *pJsonAddress = NULL;
                pJsonAddress = cJSON_GetObjectItem(pJsonRoot, "message");
                printf("message: %s\r\n", pJsonAddress->valuestring);
                pJsonAddress = cJSON_GetObjectItem(pJsonRoot, "data");
                cJSON *pNextAddress = cJSON_GetObjectItem(pJsonAddress, "key");
                printf("key: %d\r\n", pNextAddress->valueint);
            }
        }
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(TAG, "HTTP会话完成");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP已断开");
        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    }
    return ESP_OK;
}

void task_tcp_client(void *arg)
{
    wifi_sta_connect(&wp);
    struct HTTP_REQUEST req1 = {
        .url = "http://www.example.com/",
        .port = 80,
        // .url = "http://192.168.10.57:8000",
        // .port = 8000,
        .mode = HTTP_METHOD_GET,
        .http_cb = http_event_handler,
    };

    http_request(&req1);
    while (1)
    {
        vTaskDelay(100 / portTICK);
    }
}

void app_main()
{
    xTaskCreate(task_tcp_client, "task_tcp_client", 1024 * 4, NULL, 5, &wifi_handle);
    vTaskDelete(NULL);
}
