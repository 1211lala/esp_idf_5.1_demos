#include "gpio.h"
#include "gpio_int.h"

#define keypin 0
TaskHandle_t task_int_handle = NULL;
QueueHandle_t isr_quene = NULL;

void task_int(void *arg)
{
    uint8_t pin = 0;
    while (1)
    {
        if (xQueueReceive(isr_quene, &pin, portMAX_DELAY) == pdTRUE)
        {
            ESP_LOGI("INT", "GPIO[%d]INT", pin);
        }
    }
}

void IRAM_ATTR gpio_isr_callback(void *arg)
{
    BaseType_t *pxHigherPriorityTaskWoken = pdFALSE;
    uint8_t gpio_pin = (uint8_t)arg;
    switch (gpio_pin)
    {
    case 0:
        xQueueSendFromISR(isr_quene, &gpio_pin, &pxHigherPriorityTaskWoken);
        break;
    default:
        break;
    }
}

void app_main(void)
{
    gpio_general_init(GPIO_NUM_2, GPIO_MODE_INPUT_OUTPUT, true);
    gpio_isr_init(GPIO_NUM_0, GPIO_INTR_ANYEDGE, true, gpio_isr_callback, (void *)0);

    isr_quene = xQueueCreate(2, sizeof(uint8_t));
    if (isr_quene == NULL)
    {
        ESP_LOGE("main", "queue create fail.");
    }
    xTaskCreatePinnedToCore(task_int, "task_int", 1024 * 4, NULL, 2, &task_int_handle, 1);
    vTaskDelete(NULL);
}
