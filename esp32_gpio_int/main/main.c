#include "gpio.h"
#include "gpio_int.h"

#define KEYPIN1 0
#define KEYPIN2 25
#define KEYPIN3 26
#define KEYPIN4 27
#define LEDPIN 2

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
            gpio_tiggle(LEDPIN);
        }
    }
}

void IRAM_ATTR gpio_isr_callback(void *arg)
{
    BaseType_t *pxHigherPriorityTaskWoken = pdFALSE;
    uint8_t gpio_pin = (uint8_t)arg;

    switch (gpio_pin)
    {
    case KEYPIN1:
        xQueueSendFromISR(isr_quene, &gpio_pin, &pxHigherPriorityTaskWoken);
        break;
    case KEYPIN2:
        xQueueSendFromISR(isr_quene, &gpio_pin, &pxHigherPriorityTaskWoken);
        break;
    case KEYPIN3:
        xQueueSendFromISR(isr_quene, &gpio_pin, &pxHigherPriorityTaskWoken);
        break;
    case KEYPIN4:
        xQueueSendFromISR(isr_quene, &gpio_pin, &pxHigherPriorityTaskWoken);
        break;
    default:
        break;
    }
}

void app_main(void)
{
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
    gpio_general_init(LEDPIN, GPIO_MODE_INPUT_OUTPUT, true);
    gpio_isr_init(KEYPIN1, GPIO_INTR_NEGEDGE, true, gpio_isr_callback, (void *)KEYPIN1);
    gpio_isr_init(KEYPIN2, GPIO_INTR_NEGEDGE, true, gpio_isr_callback, (void *)KEYPIN2);
    gpio_isr_init(KEYPIN3, GPIO_INTR_NEGEDGE, true, gpio_isr_callback, (void *)KEYPIN3);
    gpio_isr_init(KEYPIN4, GPIO_INTR_NEGEDGE, true, gpio_isr_callback, (void *)KEYPIN4);
    isr_quene = xQueueCreate(2, sizeof(uint8_t));
    if (isr_quene == NULL)
    {
        ESP_LOGE("main", "queue create fail.");
    }
    xTaskCreatePinnedToCore(task_int, "task_int", 1024 * 4, NULL, 2, &task_int_handle, 1);
    vTaskDelete(NULL);
}
