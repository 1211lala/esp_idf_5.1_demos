#ifndef _HTTPCLIENT_H_
#define _HTTPCLIENT_H_

#include "datafile.h"
#include "esp_http_client.h"
#include "cJSON.h"

struct HTTP_REQUEST
{
    esp_http_client_method_t mode;
    uint16_t port;
    char *url;
    http_event_handle_cb http_cb;
};

int http_request(struct HTTP_REQUEST *req);

#endif