#ifndef _MAIN_H_
#define _MAIN_H_
#include "datafile.h"
#include "wifi_sta.h"
#include "http_server.h"
void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

esp_err_t http_index_cb(httpd_req_t *req);

#endif