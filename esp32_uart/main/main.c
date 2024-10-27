
#include "common.h"
#include "uart.h"

TaskHandle_t task_uart_handle = NULL;
uart_event_t event;
struct UART_PARAM u1_para;
void task_uart(void *arg)
{
    u1_para.baud = 115200;
    u1_para.tx = 25;
    u1_para.rx = 26;
    u1_para.uart_num = UART_NUM_1;

    uart_init(&u1_para);
    while (1)
    {
        if (xQueueReceive(u1_para.queue, (void *)&event, portMAX_DELAY) == pdTRUE)
        {
            bzero(u1_para.buff, UART_BUFF_SIZE);
            switch (event.type)
            {
            case UART_DATA:
                int len = uart_read_bytes(u1_para.uart_num, u1_para.buff, event.size, portMAX_DELAY);
                if (len)
                {
                    u1_para.buff[len] = '\0';
                    char temp[1024];
                    sprintf(temp, "rec-->%s\r\n", u1_para.buff);
                    uart_write_bytes(u1_para.uart_num, temp, strlen(temp));
                }
                break;
            default:
                break;
            }
        }
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    xTaskCreatePinnedToCore(task_uart, "task_uart", 1024 * 4, NULL, 5, &task_uart_handle, 1);
    vTaskDelete(NULL);
}
