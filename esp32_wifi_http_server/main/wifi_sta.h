#ifndef _WIFI_STA_H_
#define _WIFI_STA_H_

#include "datafile.h"

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_netif_ip_addr.h>
#include "esp_netif.h"
#include <nvs_flash.h>
#include <arpa/inet.h>
#include "lwip/sockets.h"

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
    uint8_t retry_num;
    bool is_connect;
};

int wifi_sta_connect(struct WIFI_PARAM *wp);
void wifi_sta_scan_connect(struct WIFI_PARAM *wp);

void print_auth_mode(int authmode);
void print_cipher_type(int pairwise_cipher, int group_cipher);
#endif