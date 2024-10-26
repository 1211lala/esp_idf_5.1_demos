
#include "common.h"
#include "uart.h"

TaskHandle_t task_uart_handle = NULL;

void task_read_uart1(void *arg)
{
    uart_event_t event;

    char *rec = (char *)malloc(100);
    char *send = (char *)malloc(200);

    uart1_init(115200, 25, 26);
    while (1)
    {
        if (xQueueReceive(uart1_queue, (void *)&event, portMAX_DELAY) == pdTRUE)
        {
            bzero(rec, 100);
            switch (event.type)
            {
            case UART_DATA:
                int len = uart_read_bytes(UART_NUM_1, rec, event.size, portMAX_DELAY);
                if (len)
                {
                    rec[len] = '\0';
                    sprintf(send, "rec-->%s\r\n", rec);
                    uart_write_bytes(UART_NUM_1, send, strlen(send));
                }
                break;
            default:
                break;
            }
        }
    }
    free(rec);
    free(send);
    vTaskDelete(NULL);
}

void app_main(void)
{
    xTaskCreatePinnedToCore(task_read_uart1, "task_read_uart1", 1024 * 4, NULL, 10, &task_uart_handle, 1);
    vTaskDelete(NULL);
}
