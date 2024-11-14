#include "http_client.h"

static const char *TAG = "http_client";

/******************************************************************************
 * 函数描述: 将pin0设置为中断
 * 参  数1: 中断的回调函数,函数类型  void (*gpio_isr_t)(void *arg);
 * 参  数2: 中断函数携带的参数
 *******************************************************************************/

int http_request(struct HTTP_REQUEST *req)
{
    esp_http_client_config_t config = {
        .url = req->url,
        .port = req->port,
        .method = req->mode,
        .event_handler = req->http_cb,
        .buffer_size = 2048,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "请求成功  CODE = %d, LEN = %llu", esp_http_client_get_status_code(client), esp_http_client_get_content_length(client));
    }
    else
    {
        ESP_LOGE(TAG, "请求失败: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return ESP_FAIL;
    }
    esp_http_client_cleanup(client);
    return ESP_OK;
}