#ifndef _MAIN_H_
#define _MAIN_H_
#include "datafile.h"
#include "wifi_sta.h"
#include "http_client.h"
void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

#endif