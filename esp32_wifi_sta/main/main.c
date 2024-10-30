#include "main.h"

TaskHandle_t wifi_handle;

struct WIFI_PARAM wp = {
    .esp_event_cb = wifi_event_handler,
    .retry_num = 0,
    .info.ssid = "艾可舒",
    .info.password = "ai666888",
};
void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    struct WIFI_PARAM *w = arg;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        w->retry_num = 0;
        ESP_LOGI("wifi_event_handler", "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI("wifi_event_handler", "Got GW: " IPSTR, IP2STR(&event->ip_info.gw));
        ESP_LOGI("wifi_event_handler", "Got NT: " IPSTR, IP2STR(&event->ip_info.netmask));

        xEventGroupSetBits(w->event_group, WIFI_CONNECTED_BIT);
    }

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (w->retry_num < 100)
        {
            esp_wifi_connect();
            w->retry_num++;
            ESP_LOGW("wifi_event_handler", "%s 重新开始连接AP %d/%d", w->info.ssid, w->retry_num, 100);
        }
        else
        {
            xEventGroupSetBits(w->event_group, WIFI_FAIL_BIT);
            ESP_LOGE("wifi_event_handler", "%s 连接失败", w->info.ssid);
        }
    }
}

void task_wifi(void *arg)
{
    wifi_sta_connect(&wp);

    wifi_config_t config;
    esp_wifi_get_config(WIFI_IF_STA, &config);
    printf("ssid: %s    password: %s\r\n", (char *)config.sta.ssid, (char *)config.sta.password);

    esp_netif_ip_info_t ip_info;
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    esp_netif_get_ip_info(netif, &ip_info);
    printf("ip: " IPSTR " gateway: " IPSTR "  subnet: " IPSTR "\r\n", IP2STR(&ip_info.ip), IP2STR(&ip_info.gw), IP2STR(&ip_info.netmask));

    esp_netif_dns_info_t dsn_info;
    esp_netif_get_dns_info(netif, ESP_NETIF_DNS_MAIN, &dsn_info);
    printf("dns: " IPSTR "\r\n", IP2STR(&dsn_info.ip.u_addr.ip4));

    while (1)
    {
        static uint16_t number = 10;
        uint16_t ap_count = 0;
        wifi_ap_record_t ap_info[10];
        memset(ap_info, 0, sizeof(ap_info));

        esp_wifi_scan_start(NULL, true);
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));

        ESP_LOGW("wifi_sta_scan_connect", "Total APs scanned = %d", ap_count);

        ESP_LOGW("wifi_sta_scan_connect", "%-30s%-30s%-30s", "ssid", "rssi", "channel");

        for (int i = 0; (i < number) && (i < ap_count); i++)
        {
            ESP_LOGI("wifi_sta_scan_connect", "%-30s  %-30d  %-30d", ap_info[i].ssid, ap_info[i].rssi, ap_info[i].primary);
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS); // 每5秒扫描一次
    }
}

void app_main()
{
    xTaskCreate(task_wifi, "task_wifi", 1024 * 4, NULL, 5, &wifi_handle);
    vTaskDelete(NULL);
}
