#include "main.h"

TaskHandle_t wifi_handle;

struct WIFI_PARAM wp = {
    .esp_event_cb = wifi_event_handler,
    .retry_num = 0,
    // .info.ssid = "艾可舒",
    // .info.password = "ai666888",
    .info.ssid = "Xiaomi_4C",
    .info.password = "121314liuAO",
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

const char *index_content =
    "<!DOCTYPE html>\n"
    "<html lang=\"zh-CN\">\n"
    "<head>\n"
    "    <meta charset=\"UTF-8\">\n"
    "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
    "    <title>黄红色背景</title>\n"
    "    <style>\n"
    "        /* 将页面背景设置为黄红色渐变 */\n"
    "        body {\n"
    "            margin: 0;\n"
    "            height: 100vh;\n"
    "            display: flex;\n"
    "            justify-content: center;\n"
    "            align-items: center;\n"
    "            background: linear-gradient(45deg, yellow, red);\n"
    "            color: white;\n"
    "            font-family: Arial, sans-serif;\n"
    "        }\n"
    "\n"
    "        h1 {\n"
    "            font-size: 2em;\n"
    "            text-align: center;\n"
    "        }\n"
    "    </style>\n"
    "</head>\n"
    "<body>\n"
    "    <h1>黄红色渐变背景页面</h1>\n"
    "</body>\n"
    "</html>\n";
const char *led_content =
    "<!DOCTYPE html>\n"
    "<html lang=\"zh-CN\">\n"
    "<head>\n"
    "    <meta charset=\"UTF-8\">\n"
    "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
    "    <title>黄红色背景</title>\n"
    "    <style>\n"
    "        /* 将页面背景设置为黄红色渐变 */\n"
    "        body {\n"
    "            margin: 0;\n"
    "            height: 100vh;\n"
    "            display: flex;\n"
    "            justify-content: center;\n"
    "            align-items: center;\n"
    "            background: linear-gradient(45deg, green, red);\n"
    "            color: white;\n"
    "            font-family: Arial, sans-serif;\n"
    "        }\n"
    "\n"
    "        h1 {\n"
    "            font-size: 2em;\n"
    "            text-align: center;\n"
    "        }\n"
    "    </style>\n"
    "</head>\n"
    "<body>\n"
    "    <h1>绿红色渐变背景页面</h1>\n"
    "</body>\n"
    "</html>\n";

const char *jsonString = "{\n"
                         "  \"id\": 1,\n"
                         "  \"message\": \"这是一个测试字符串\",\n"
                         "  \"timestamp\": \"2024-11-15T10:30:00Z\"\n"
                         "}";

esp_err_t http_index_cb(httpd_req_t *req)
{
    if (0 == strcmp((const char *)req->user_ctx, "/"))
    {
        httpd_resp_send(req, index_content, strlen(index_content));
    }
    else if (0 == strcmp((const char *)req->user_ctx, "/led"))
    {
        httpd_resp_send(req, led_content, strlen(led_content));
    }
    else if (0 == strcmp((const char *)req->user_ctx, "/json"))
    {
        /* 设置返回码 */
        httpd_resp_set_status(req, "200 OK");
        /* 设置 Content-Type 头数据 "application/json"、"text/html" */
        httpd_resp_set_type(req, "application/json");
        /* 多功能函数 可以设置  Content-Type "Authorization" 等头部数据 */
        // httpd_resp_set_hdr(req, "Content-Type", "application/json");
        /* 默认设置 code:200响应码 text/html类型 */
        httpd_resp_send(req, jsonString, strlen(jsonString));
    }
    ESP_LOGI("http_index_cb", "%s", (char *)req->user_ctx);
    return ESP_OK;
}

struct HTTP_URL http_server = {

    .port = 8000,
};
httpd_uri_t root_url = {

    .handler = http_index_cb,
    .method = HTTP_GET,
    .uri = "/",
    .user_ctx = "/",
};

httpd_uri_t led_url = {

    .handler = http_index_cb,
    .method = HTTP_GET,
    .uri = "/led",
    .user_ctx = "/led",
};
httpd_uri_t json_url = {

    .handler = http_index_cb,
    .method = HTTP_GET,
    .uri = "/json",
    .user_ctx = "/json",
};
void task_http_server(void *arg)
{
    wifi_sta_connect(&wp);
    http_start(&http_server);
    http_register_uri(&http_server, &root_url);
    http_register_uri(&http_server, &led_url);
    http_register_uri(&http_server, &json_url);
    ESP_LOGI("task_http_server", "http start!!!");
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
