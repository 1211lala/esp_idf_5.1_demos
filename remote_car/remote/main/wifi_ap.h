#ifndef _WIFI_AP_H_
#define _WIFI_AP_H_

#include "datafile.h"

#include <nvs_flash.h>
#include <esp_event.h>
#include <esp_wifi.h>
#include <esp_netif_ip_addr.h>
#include "esp_netif.h"
#include <arpa/inet.h>

#define AP_IS_START 1 << 0
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
    struct WIFI_INFO info;            // wifi信息
    EventGroupHandle_t event_group;   // 事件组
    esp_event_handler_t esp_event_cb; // wifi回调
    uint8_t retry_num;                // 重连次数, sta模式下使用
    bool is_connect;                  // wifi是否连接成功, sta模式下使用
    uint8_t connects;                 // sta连接数, ap模式下使用
};
extern struct WIFI_PARAM ap;

void wifi_ap_start(struct WIFI_PARAM *param);
void ap_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
#endif