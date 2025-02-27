/*
# ESP32的串口接收方式
    当 UART 接收到数据时，ESP32 的 UART 驱动程序会触发中断，并将接收到的数据放入接收缓冲区。
    然后，中断服务程序会将接收到的数据从接收缓冲区中读取，并将其放入预先创建的队列中。
    这样，任务可以从队列中读取接收到的数据，而不需要在中断服务程序中直接处理接收到的数据


uart_driver_install() 函数是 ESP32 中用于安装 UART 驱动程序的函数，用于配置和启动 UART 控制器
这个函数的作用是初始化和启动 UART 驱动程序，并为 UART 实例分配必要的资源。它设置了 UART 的接收缓冲区和发送缓冲区的大小，并根据需要创建了一个用于接收数据的队列。
当接收到数据时，驱动程序将数据存储在接收缓冲区中，并通过中断将接收到的数据放入队列中。

esp_err_t uart_driver_install(uart_port_t uart_num, int rx_buffer_size, int tx_buffer_size, int queue_size, QueueHandle_t *uart_queue, int intr_alloc_flags)

uart_port_t uart_num：UART 口号，可以是 UART_NUM_0 或 UART_NUM_1。
int rx_buffer_size：接收缓冲区大小，以字节为单位。接收到的数据将存储在这个缓冲区中，直到被读取。
int tx_buffer_size：发送缓冲区大小，以字节为单位。要发送的数据将存储在这个缓冲区中，直到被发送。
int queue_size：UART 驱动程序将用于接收数据的队列大小。如果设置为 0，则不创建接收队列。
QueueHandle_t *uart_queue：用于接收数据的队列句柄。如果 queue_size 参数大于 0，则该参数必须为有效的队列句柄，否则应该为 NULL。
int intr_alloc_flags：中断分配标志，用于指定中断服务程序的配置。可以使用 ESP_INTR_FLAG_IRAM 将中断服务程序分配到 IRAM 中，以提高中断响应速度。


intr_alloc_flags 参数用于指定中断服务程序的配置标志。：

ESP_INTR_FLAG_LEVEL1、ESP_INTR_FLAG_LEVEL2、ESP_INTR_FLAG_LEVEL3、ESP_INTR_FLAG_LEVEL4：这些标志用于指定中断服务程序的优先级，
分别对应着中断的优先级 1 到 4。优先级 1 最高，4 最低。默认优先级为 1。
优先级较高的中断服务程序可以打断优先级较低的中断服务程序，但不能打断同优先级或更高优先级的中断服务程序。

ESP_INTR_FLAG_EDGE：指定中断为边沿触发模式。在边沿触发模式下，中断服务程序会在引脚的上升沿或下降沿触发。

ESP_INTR_FLAG_IRAM：指定将中断服务程序分配到 IRAM（内部 RAM）中。分配中断服务程序到 IRAM 中可以提高中断的响应速度，但会占用 IRAM 的空间。

ESP_INTR_FLAG_SHARED：指定中断为共享中断。在共享中断模式下，多个硬件模块可以共享同一个中断。

ESP_INTR_FLAG_INTRDISABLED：指定中断服务程序在初始化时不会被使能。这可以用于在初始化完成后手动使能中断服务程序。

这些标志可以按位或（|）操作进行组合，以满足特定的中断需求。例如，可以使用 ESP_INTR_FLAG_LEVEL3 | ESP_INTR_FLAG_IRAM 来指定中断的优先级为 3，并将中断服务程序分配到 IRAM 中。
*/

#include "uart.h"
/******************************************************************************
 * 函数描述: 串口初始化 8N1,无硬件流控，在接收完成后，通过queue启动接收任务,设置queue设置为 5个缓冲区,大小为 512byte
 *          为了提高接收的及时性，有必要把接收任务的优先级提高
 *******************************************************************************/
void uart_init(struct UART_PARAM *uart_para)
{

    uart_config_t uart_cfg = {};
    /* 分配内存 */
    uart_para->buff = malloc(UART_BUFF_SIZE); // 注意这里使用箭头操作符
    uart_cfg.baud_rate = uart_para->baud;
    uart_cfg.data_bits = UART_DATA_8_BITS;
    uart_cfg.parity = UART_PARITY_DISABLE;
    uart_cfg.stop_bits = UART_STOP_BITS_1;
    uart_cfg.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_cfg.source_clk = UART_SCLK_DEFAULT;

    ESP_ERROR_CHECK(uart_param_config(uart_para->uart_num, &uart_cfg));

    ESP_ERROR_CHECK(uart_driver_install(uart_para->uart_num, UART_RX_SIZE, UART_TX_SIZE, UART_QUEUE_SIZE, &uart_para->queue, ESP_INTR_FLAG_LEVEL3 | ESP_INTR_FLAG_IRAM));

    ESP_ERROR_CHECK(uart_set_pin(uart_para->uart_num, uart_para->tx, uart_para->rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

void uart_reset(struct UART_PARAM *uart_para)
{
    uart_driver_delete(uart_para->uart_num);
    free(uart_para->buff);
}