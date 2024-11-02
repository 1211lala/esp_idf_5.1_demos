#include "datafile.h"
#include "adc.h"

TaskHandle_t task_adc_handle = NULL;

struct ADC_ONESHOT adc1 = {
    .unit_id = ADC_UNIT_1,
};
struct ADC_CHANNEL adc1_chan_5 = {
    .channel = ADC_CHANNEL_5,
};
void task_adc(void *arg)
{
    adc_oneshot_init(&adc1);
    adc_oneshot_cfg_channel(&adc1, &adc1_chan_5);
    while (1)
    {
        ESP_ERROR_CHECK(adc_oneshot_read(adc1.handle, adc1_chan_5.channel, &adc1_chan_5.value));
        if (adc1_chan_5.cali_result)
        {
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_chan_5.cali_handle, adc1_chan_5.value, &adc1_chan_5.value_mv));
        }
        ESP_LOGI("task_adc", "adc1_chan_5.value: %d     %dmv", adc1_chan_5.value, adc1_chan_5.value_mv);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void app_main(void)
{
    xTaskCreatePinnedToCore(task_adc, "task_adc", 1024 * 4, NULL, 2, &task_adc_handle, 1);
    vTaskDelete(NULL);
}
