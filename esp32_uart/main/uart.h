#ifndef _UART_H_
#define _UART_H_

#include "common.h"
#include "driver/uart.h"

#define UART_BUFF_SIZE 512
#define UART_RX_SIZE 512
#define UART_TX_SIZE 0
#define UART_QUEUE_SIZE 5
struct UART_PARAM
{
    uint8_t uart_num;    /* 串口编号 */
    uint32_t baud;       /* 波特率 */
    uint8_t tx;          /* tx引脚 */
    uint8_t rx;          /* rx引脚 */
    QueueHandle_t queue; /* 与串口数据关联的queue */
    uint8_t *buff;       /* 缓存,需要手动分配内存 */
};

void uart_init(struct UART_PARAM *uart_para);
void uart_reset(struct UART_PARAM *uart_para);

#endif