#include "main.h"

#include "lwip/sockets.h"

TaskHandle_t wifi_handle;

struct WIFI_PARAM wp = {
    .esp_event_cb = wifi_event_handler,
    .retry_num = 0,
    .info.ssid = "艾可舒",
    .info.password = "ai666888",
    .is_connect = false,
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
        w->is_connect = true;
        xEventGroupSetBits(w->event_group, WIFI_CONNECTED_BIT);
    }
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        w->is_connect = false;
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

struct CLIENT
{
    int fd;
    struct sockaddr_in sockaddr;
};

void task_tcp_server_fun(void *arg)
{
    struct CLIENT *client = (struct CLIENT *)arg;
    char ip_buf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client->sockaddr.sin_addr.s_addr, ip_buf, INET_ADDRSTRLEN);
    ESP_LOGI("task_tcp_server", "remote: %s:%d connect\n", ip_buf, ntohs(client->sockaddr.sin_port));

    char r_buf[100];
    char w_buf[200];
    while (1)
    {
        ssize_t ret = recv(client->fd, r_buf, sizeof(r_buf), 0);
        if (ret <= 0)
        {
            break;
        }
        r_buf[ret] = '\0';
        sprintf(w_buf, "rec->%s\n", r_buf);
        send(client->fd, w_buf, strlen(w_buf), 0);
    }
    ESP_LOGW("task_tcp_server", "remote: %s:%d disconnect\n", ip_buf, ntohs(client->sockaddr.sin_port));
    close(client->fd);
    free(client);
    vTaskDelete(NULL);
}

void task_tcp_server(void *arg)
{
    wifi_sta_connect(&wp);
    esp_netif_ip_info_t ip_info;
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    esp_netif_get_ip_info(netif, &ip_info);
    /* 新建网络描述符 */
    int hostfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    struct sockaddr_in listen_sockaddr = {};
    listen_sockaddr.sin_family = AF_INET;
    listen_sockaddr.sin_addr.s_addr = ip_info.ip.addr; /* 将本机IP设置为监听IP */
    listen_sockaddr.sin_port = htons(8000);            /* 监听端口 */
    /* 绑定 */
    if (0 != bind(hostfd, (struct sockaddr *)&listen_sockaddr, sizeof(struct sockaddr_in)))
    {
        ESP_LOGE("task_tcp_server", "bind fail !!!");
    }
    if (0 != listen(hostfd, 3))
    {
        ESP_LOGE("task_tcp_server", "listen fail !!!");
    }
    socklen_t len = sizeof(struct sockaddr_in);
    while (1)
    {
        /* 等待连接 */
        struct CLIENT *client = malloc(sizeof(struct CLIENT));

        client->fd = accept(hostfd, (struct sockaddr *)&client->sockaddr, &len);
        if (client->fd < 0)
        {
            free(client);
            ESP_LOGE("task_tcp_server", "accept fail !!!");
        }
        xTaskCreate(task_tcp_server_fun, "task_tcp_server_fun", 1024 * 4, client, 4, NULL);
    }
    close(hostfd);
}

void app_main()
{
    xTaskCreate(task_tcp_server, "task_tcp_server", 1024 * 4, NULL, 5, &wifi_handle);
    vTaskDelete(NULL);
}
