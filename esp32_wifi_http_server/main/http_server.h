#ifndef _HTTPCLIENT_H_
#define _HTTPCLIENT_H_

#include "datafile.h"
#include <esp_http_server.h>
#include "cJSON.h"

struct HTTP_URL
{
    httpd_handle_t server;
    uint16_t port;
};

int http_register_uri(struct HTTP_URL *hu, const httpd_uri_t *url);

int http_unregister_uri(struct HTTP_URL *hu, const httpd_uri_t *url);

#endif