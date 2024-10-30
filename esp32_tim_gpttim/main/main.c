
#include "common.h"
#include "gpttim.h"

QueueHandle_t gpt1_isr_queue = NULL;
QueueHandle_t gpt2_isr_queue = NULL;

void IRAM_ATTR gptim1_task(void *arg)
{
    while (1)
    {
        uint64_t value = 0;
        if (xQueueReceive(gpt1_isr_queue, &value, portMAX_DELAY))
        {
            ESP_LOGI("gptim1_task", "gptim1 value : %llu", value);
        }
    }
}

void IRAM_ATTR gptim2_task(void *arg)
{
    while (1)
    {
        uint64_t value = 0;
        if (xQueueReceive(gpt2_isr_queue, &value, portMAX_DELAY))
        {
            ESP_LOGI("gptim2_task", "gptim2 value : %llu", value);
        }
    }
}

uint64_t cnt1 = 0;
uint64_t cnt2 = 0;
bool IRAM_ATTR gpttim_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    /* 也可以使用 gptimer_handle_t timer 来做定时器判断, 这里使用自定义变量来做判断 */
    struct GPTTIM_PARAM *gpt = user_data;
    switch (gpt->id)
    {
    case 1:
        cnt1++;
        xQueueSendFromISR(gpt1_isr_queue, &cnt1, &pxHigherPriorityTaskWoken);
        break;
    case 2:
        cnt2++;
        xQueueSendFromISR(gpt2_isr_queue, &cnt2, &pxHigherPriorityTaskWoken);
        break;
    default:
        break;
    }

    return pxHigherPriorityTaskWoken;
}

struct GPTTIM_PARAM gpt1 = {
    .freq = 1000000,
    .target_cnt = 100000,
    .gpttimer_cb = gpttim_on_alarm_cb,
    .arg = &gpt1,
    .id = 1,
};
struct GPTTIM_PARAM gpt2 = {
    .freq = 1000000,
    .target_cnt = 1000000,
    .gpttimer_cb = gpttim_on_alarm_cb,
    .arg = &gpt2,
    .id = 2,
};

void app_main(void)
{
    gpt1_isr_queue = xQueueCreate(2, sizeof(uint64_t));
    gpt2_isr_queue = xQueueCreate(2, sizeof(uint64_t));
    if (gpt1_isr_queue == NULL)
    {
        ESP_LOGE("app_main", "gpt1_isr_queue init fail");
    }
    if (gpt2_isr_queue == NULL)
    {
        ESP_LOGE("app_main", "gpt2_isr_queue init fail");
    }
    gpttim_init(&gpt1);
    gpttim_init(&gpt2);
    gpttim_start(&gpt1);
    gpttim_start(&gpt2);
    xTaskCreatePinnedToCore(gptim1_task, "gptim1_task", 1024 * 4, NULL, 10, NULL, 1);
    xTaskCreatePinnedToCore(gptim2_task, "gptim2_task", 1024 * 4, NULL, 10, NULL, 1);
    vTaskDelete(NULL);
}
