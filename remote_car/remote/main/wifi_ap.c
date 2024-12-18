#include "wifi_ap.h"

/*************************************************************************************
 *
 *************************************************************************************/
void wifi_ap_start(struct WIFI_PARAM *param)
{
    // 1 : 初始化 NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    { // 如果 NVS 分区已满或者版本不匹配，需要擦除 NVS 分区并重新初始化
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    // 2: 初始化网络接口
    ESP_ERROR_CHECK(esp_netif_init());

    // 3: 创建默认的事件循环
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // 4 : 创建默认的 Wi-Fi AP 网络接口
    esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();
    /*********************************设置固定IP可选******************************************/
    esp_netif_ip_info_t info_t;
    memset(&info_t, 0, sizeof(esp_netif_ip_info_t));
    if (ap_netif)
    {
        ESP_ERROR_CHECK(esp_netif_dhcps_stop(ap_netif));
        info_t.ip.addr = esp_ip4addr_aton(param->info.ip);
        info_t.netmask.addr = esp_ip4addr_aton(param->info.subnet);
        info_t.gw.addr = esp_ip4addr_aton(param->info.gateway);
        esp_netif_set_ip_info(ap_netif, &info_t);
        ESP_ERROR_CHECK(esp_netif_dhcps_start(ap_netif));
    }
    /*********************************设置固定IP可选******************************************/
    // 5: 初始化 Wi-Fi 栈，之后可以进行Wi-Fi 连接、启动 Wi-Fi 等操作。
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 6: 配置WiFi模式
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    // 7: 配置WiFi参数
    wifi_config_t ap_cfg = {};
    memcpy(ap_cfg.ap.ssid, param->info.ssid, strlen(param->info.ssid));
    memcpy(ap_cfg.ap.password, param->info.password, strlen(param->info.password));
    ap_cfg.ap.ssid_len = strlen(param->info.ssid);
    ap_cfg.ap.channel = 2;
    ap_cfg.ap.max_connection = 3;
    ap_cfg.ap.authmode = WIFI_AUTH_WPA2_PSK;
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_cfg));

    // 9: 设置WiFi事件
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, param->esp_event_cb, param, NULL);
    /*
    10: 开启WiFi，调用 esp_wifi_start() 函数后，ESP32 将开始扫描 Wi-Fi 网络并尝试连接。
        在这个过程中，系统会产生多种类型的 Wi-Fi 事件，例如扫描结果事件、连接事件等。
        这些事件会通过事件循环传递给注册的事件处理函数。
    */
    ESP_ERROR_CHECK(esp_wifi_start());
}

void ap_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    struct WIFI_PARAM *a = arg;
    if (event_id == WIFI_EVENT_AP_START)
    {
        xEventGroupSetBits(a->event_group, AP_IS_START);
        ESP_LOGI("wifi_event_handler", "AP started");
    }
    else if (event_id == WIFI_EVENT_AP_STOP)
    {
        ESP_LOGE("wifi_event_handler", "AP stopped");
    }
    else if (event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        ESP_LOGI("wifi_event_handler", "have a new device connect");
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        ESP_LOGE("wifi_event_handler", "have a device disconnect");
    }
}