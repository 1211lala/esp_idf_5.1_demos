#include "http_server.h"

static const char *TAG = "http_server";

/******************************************************************************
 * 函数描述:
 *
 *
 *******************************************************************************/

int http_start(struct HTTP_URL *hu)
{
    httpd_config_t http_cfg = HTTPD_DEFAULT_CONFIG();
    http_cfg.server_port = hu->port;
    return httpd_start(&hu->server, &http_cfg);
}
int http_register_uri(struct HTTP_URL *hu, const httpd_uri_t *url)
{
    
    return httpd_register_uri_handler(hu->server, url);
}

int http_unregister_uri(struct HTTP_URL *hu, const httpd_uri_t *url)
{
    
    return httpd_unregister_uri_handler(hu->server, url->uri, url->method);
}