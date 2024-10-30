#ifndef _WIFI_STA_H_
#define _WIFI_STA_H_

#include "datafile.h"

#include <esp_wifi.h>

#include <nvs_flash.h>
#include <esp_event.h>
#include <esp_wifi.h>
#include <esp_netif_ip_addr.h>
#include "esp_netif.h"
#include <arpa/inet.h>

#define WIFI_FAIL_BIT (1 << 0)
#define WIFI_CONNECTED_BIT (1 << 1)

struct WIFI_INFO
{
    char *ssid;
    char *password;
    char *ip;
    char *gateway;
    char *subnet;
    char *dns;
};
struct WIFI_PARAM
{
    struct WIFI_INFO info;
    EventGroupHandle_t event_group;
    esp_event_handler_t esp_event_cb;
};

extern struct WiFi_PARAM wp;
extern EventGroupHandle_t s_wifi_event_group;
void wifi_sta_connect(struct WiFi_PARAM *WiFi_Config, esp_event_handler_t esp_event_callback);


#endif