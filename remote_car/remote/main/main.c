#include "datafile.h"

#include "gpio.h"
#include "wifi_tcp.h"
#include "remote.h"
#include "adc.h"

void tcp_server_task(void *arg);
void adc_key_task(void *arg);
void led_task(void *arg);

TaskHandle_t tcpServerHandle;
TaskHandle_t adcKeyHandle;
QueueHandle_t dataQuene;

struct REMOTE_VALUE
{
    int16_t lx;
    int16_t ly;
    int16_t rx;
    int16_t ry;
};

struct ADC_CELI_VALUE
{
    uint16_t lx;
    uint16_t ly;
    uint16_t rx;
    uint16_t ry;
};

struct REMOTE_VALUE remote_value;
bool is_open_tcp = false;

struct WIFI_PARAM ap = {
    .esp_event_cb = ap_event_handler,
    .info.ssid = "esp32-car-ap",
    .info.password = "1234567890",
    .info.ip = "192.168.20.10",
    .info.gateway = "192.168.20.1",
    .info.subnet = "255.255.255.0",
    .info.dns = "114.114.114.114",
    .connects = 0,
};
void app_main()
{
    if (pdPASS != xTaskCreate(led_task, "led_task", 1024 * 4, NULL, 3, NULL))
    {
    }
    gpio_general_init(RIGHT_TOP_KEY, GPIO_MODE_INPUT, true);
    gpio_general_init(LEFT_DOWN_KEY, GPIO_MODE_INPUT, true);
    gpio_general_init(LEFT_TOP_KEY, GPIO_MODE_INPUT, true);
    gpio_general_init(CENTER_TOP_KEY, GPIO_MODE_INPUT, true);
    gpio_general_init(LED, GPIO_MODE_INPUT_OUTPUT, true);

    ap.event_group = xEventGroupCreate();
    wifi_ap_start(&ap);
    if (pdPASS != xEventGroupWaitBits(ap.event_group, AP_IS_START, pdFALSE, pdTRUE, portMAX_DELAY))
    {
    }
    ESP_LOGI("app_main", "ap is start!!!");
    vEventGroupDelete(ap.event_group);

    if (pdPASS != xTaskCreate(adc_key_task, "tcp_server_task", 1024 * 4, NULL, 5, &adcKeyHandle))
    {
    }
    if (pdPASS != xTaskCreate(tcp_server_task, "tcp_server_task", 1024 * 4, NULL, 4, &tcpServerHandle))
    {
    }
}

struct ADC_ONESHOT adc1 = {
    .unit_id = ADC_UNIT_1,
};
struct ADC_CHANNEL adc1_chan[4] = {
    {
        .channel = ADC_CHANNEL_4, // RIGHT_REMOTE_Y 32
    },
    {
        .channel = ADC_CHANNEL_0, // RIGHT_REMOTE_X 36
    },
    {
        .channel = ADC_CHANNEL_7, // LEFT_REMOTE_Y 35
    },
    {
        .channel = ADC_CHANNEL_6, // LEFT_REMOTE_X 34
    },
};

#define ERROR_RANGE 150
void adc_key_task(void *arg)
{
    /* 包含adc按键的数据队列 */
    dataQuene = xQueueCreate(4, sizeof(struct REMOTE_VALUE));

    adc_oneshot_init(&adc1);
    for (int i = 0; i < 4; i++)
    {
        adc_oneshot_cfg_channel(&adc1, &adc1_chan[i]);
    }

    /* 获得校准值 */
    struct ADC_CELI_VALUE celi_val;
    struct ADC_CELI_VALUE *celiArray = malloc(sizeof(struct ADC_CELI_VALUE) * 5);
    for (int j = 0; j < 5; j++)
    {
        for (int i = 0; i < 4; i++)
        {
            ESP_ERROR_CHECK(adc_oneshot_read(adc1.handle, adc1_chan[i].channel, &adc1_chan[i].value));
            if (adc1_chan[i].cali_result)
            {
                ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_chan[i].cali_handle, adc1_chan[i].value, &adc1_chan[i].value_mv));
            }
            vTaskDelay(pdMS_TO_TICKS(30));
        }

        celiArray[j].lx = adc1_chan[3].value;
        celiArray[j].ly = adc1_chan[2].value;
        celiArray[j].ry = adc1_chan[0].value;
        celiArray[j].rx = adc1_chan[1].value;
    }
    for (int i = 0; i < 5; i++)
    {
        printf("lx: %d  ly: %d  ry: %d  rx: %d\n", celiArray[i].lx, celiArray[i].ly, celiArray[i].ry, celiArray[i].rx);
    }
    celi_val.lx = celiArray[3].lx;
    celi_val.ly = celiArray[3].ly;
    celi_val.rx = celiArray[3].rx;
    celi_val.ry = celiArray[3].ry;

    free(celiArray);
    uint32_t cnt = 0;
    while (1)
    {
        for (int i = 0; i < 4; i++)
        {
            ESP_ERROR_CHECK(adc_oneshot_read(adc1.handle, adc1_chan[i].channel, &adc1_chan[i].value));
            if (adc1_chan[i].cali_result)
            {
                ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_chan[i].cali_handle, adc1_chan[i].value, &adc1_chan[i].value_mv));
            }
            vTaskDelay(pdMS_TO_TICKS(20));
        }

        int lx = (adc1_chan[3].value - celi_val.lx);
        int ly = (adc1_chan[2].value - celi_val.ly);
        int ry = (adc1_chan[0].value - celi_val.ry);
        int rx = (adc1_chan[1].value - celi_val.rx);
        // printf("lx: %d  ly: %d  ry: %d  rx: %d\n", remote_value.lx, remote_value.ly, remote_value.ry, remote_value.rx);
        if (ap.connects && is_open_tcp)
        {
            if (abs(lx) > ERROR_RANGE || abs(ly) > ERROR_RANGE || abs(ry) > ERROR_RANGE || abs(rx) > ERROR_RANGE)
            {
                cnt = 0;
                if (abs(lx) > ERROR_RANGE)
                {
                    if (lx > 0)
                        remote_value.lx = (lx - ERROR_RANGE) / 15;
                    else
                        remote_value.lx = (lx + ERROR_RANGE) / 15;
                }
                if (abs(ly) > ERROR_RANGE)
                {
                    if (ly > 0)
                        remote_value.ly = (ly - ERROR_RANGE) / 15;
                    else
                        remote_value.ly = (ly + ERROR_RANGE) / 15;
                }
                if (abs(ry) > ERROR_RANGE)
                {
                    if (ry > 0)
                        remote_value.ry = (ry - ERROR_RANGE) / 15;
                    else
                        remote_value.ry = (ry + ERROR_RANGE) / 15;
                }
                if (abs(rx) > ERROR_RANGE)
                {
                    if (rx > 0)
                        remote_value.rx = (rx - ERROR_RANGE) / 15;
                    else
                        remote_value.rx = (rx + ERROR_RANGE) / 15;
                }
                xQueueSend(dataQuene, &remote_value, 0);
            }
            else
            {
                if (cnt < 2)
                {
                    cnt += 1;
                    remote_value.lx = 0;
                    remote_value.ly = 0;
                    remote_value.ry = 0;
                    remote_value.rx = 0;
                    xQueueSend(dataQuene, &remote_value, 0);
                }
            }
        }
    }
}

/* aa ff 01 01 ff aa  */

void tcp_server_task(void *arg)
{
    /* 新建网络描述符 */
    int hostfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    struct sockaddr_in listen_sockaddr = {};
    listen_sockaddr.sin_family = AF_INET;
    inet_aton(ap.info.ip, listen_sockaddr.sin_addr.s_addr); /* 将本机IP设置为监听IP */
    listen_sockaddr.sin_port = htons(8000);                 /* 监听端口 */
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
wait_client:
    struct sockaddr_in clientsock;
    int clientfd = accept(hostfd, (struct sockaddr *)&clientsock, &len);
    is_open_tcp = true;
    printf("%s\n", inet_ntoa(clientsock.sin_addr.s_addr));

    uint8_t r_buf[100];

    struct REMOTE_VALUE val;
    while (1)
    {
        if (pdPASS == xQueueReceive(dataQuene, &val, portMAX_DELAY))
        {
            sprintf((char *)r_buf, "lx: %d  ly: %d  ry: %d  rx: %d\n", remote_value.lx, remote_value.ly, remote_value.ry, remote_value.rx);
            printf("%s", r_buf);

            r_buf[0] = 0xaa;
            r_buf[1] = 0xff;
            r_buf[2] = 0x80 + remote_value.ry; // 车速
            if (r_buf[2] > 255)
                r_buf[2] = 255;
            if (r_buf[2] < 0)
                r_buf[2] = 0;

            r_buf[3] = 0x80 + remote_value.lx; // 车速
            if (r_buf[3] > 255)
                r_buf[3] = 255;
            if (r_buf[3] < 0)
                r_buf[3] = 0;
            r_buf[4] = 0xff;
            r_buf[5] = 0xaa;

            int ret = send(clientfd, r_buf, 6, 0);
            if (ret <= 0)
            {
                is_open_tcp = false;
                close(clientfd);
                goto wait_client;
            }
        }
    }
    close(hostfd);

    // ssize_t ret = recv(clientfd, r_buf, sizeof(r_buf), 0);
}

void led_task(void *arg)
{
    while (1)
    {
        if (ap.connects == 0)
        {
            gpio_set_level(LED, false);
            vTaskDelay(300 / portTICK);
        }
        if (ap.connects && is_open_tcp == 0)
        {
            gpio_toggle(LED);
            vTaskDelay(700 / portTICK);
        }
        if (ap.connects && is_open_tcp)
        {
            gpio_set_level(LED, true);
            vTaskDelay(1000 / portTICK);
        }
    }
}