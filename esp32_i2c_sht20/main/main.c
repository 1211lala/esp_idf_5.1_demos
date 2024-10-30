#include "common.h"
#include "sht20.h"

void sht20_task(void *pvParameters)
{
    sht20_init();
    float temp = 0;
    float hum = 0;
    uint8_t flag = 0;
    while (1)
    {
        flag = 0;
        if (sht20_get_temp(&temp) != ESP_OK)
        {
            flag = 1;
        }
        if (sht20_get_hum(&hum) != ESP_OK)
        {
            flag = 1;
        }
        if (flag == 0)
        {
            ESP_LOGI("SHT20", "HUM: %.2f%% TEMP: %.2fC", hum, temp);
        }
    }
}

void app_main()
{
    xTaskCreate(sht20_task, "sht20_task", 1024 * 4, NULL, 5, NULL);
}
