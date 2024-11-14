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

esp_err_t http_index_cb(httpd_req_t *req)
{
    // static const char *path = "/spiffs/html.html";
    // char *buff = NULL;
    // int len = fs_read(path, &buff, 1024 * 8);
    // if (len > 0)
    // {
    //     ESP_LOGI(TAG, "从%s总读取%d字节的数据", path, len);
    //     httpd_resp_send(req, buff, len);
    //     free(buff);
    // }
    // else
    // {
    //     httpd_resp_send(req, "OPEN File Error", strlen("OPEN File Error"));
    // }
    ESP_LOGE("http", "%s", (char *)req->user_ctx);
    return ESP_OK;
}

struct HTTP_URL http_server = {

    .port = 8000,
};
httpd_uri_t index_rul = {

    .handler = http_index_cb,
    .method = HTTP_GET,
    .uri = "/",
    .user_ctx = "index_rul",
};
void task_http_server(void *arg)
{
    wifi_sta_connect(&wp);
    http_register_uri(&http_server, &index_rul);
    while (1)
    {
        vTaskDelay(100 / portTICK);
    }
}

void app_main()
{
    xTaskCreate(task_http_server, "task_http_server", 1024 * 4, NULL, 5, &wifi_handle);
    vTaskDelete(NULL);
}
