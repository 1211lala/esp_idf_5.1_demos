#include "http_server.h"

static const char *TAG = "http_server";

/******************************************************************************
 * 函数描述:
 *
 *
 *******************************************************************************/

int http_register_uri(struct HTTP_URL *hu, const httpd_uri_t *url)
{
    httpd_config_t http_cfg = HTTPD_DEFAULT_CONFIG();
    http_cfg.server_port = hu->port;
    if (httpd_start(&hu->server, &http_cfg) == ESP_OK)
    {
        ESP_LOGI(TAG, "开启成功");
        httpd_register_uri_handler(hu->server, url);
    }
    else
    {
        ESP_LOGE(TAG, "开启失败");
        return -1;
    }
    return ESP_OK;
}

int http_unregister_uri(struct HTTP_URL *hu, const httpd_uri_t *url)
{
    httpd_unregister_uri_handler(hu->server, url->uri, url->method);
    return ESP_OK;
}