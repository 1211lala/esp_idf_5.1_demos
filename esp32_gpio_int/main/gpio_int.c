#include "gpio_int.h"

/******************************************************************************
 * 函数描述: 将引脚设置为中断触发
 * gpio_num_t: GPIO_NUM_X
 * gpio_int_type_t:     GPIO_INTR_DISABLE = 0,      Disable GPIO interrupt
                        GPIO_INTR_POSEDGE = 1,      GPIO interrupt type : rising edge
                        GPIO_INTR_NEGEDGE = 2,      GPIO interrupt type : falling edge
                        GPIO_INTR_ANYEDGE = 3,      GPIO interrupt type : both rising and falling edge
                        GPIO_INTR_LOW_LEVEL = 4,    GPIO interrupt type : input low level trigger
                        GPIO_INTR_HIGH_LEVEL = 5,   GPIO interrupt type : input high level trigger
                        GPIO_INTR_MAX,
 * is_pull_up: 是否上拉
 * isr_handler:回调函数
 * arg: 用户参数
 *
 *
 * gpio_install_isr_service 与 gpio_isr_register 的区别 https://blog.csdn.net/qq_24317435/article/details/113497368
 *
 *******************************************************************************/
void gpio_isr_init(gpio_num_t gpio_num, gpio_int_type_t gpio_int_type, bool is_pull_up, gpio_isr_t isr_handler, void *arg)
{

    gpio_config_t pin_cfg = {};

    pin_cfg.intr_type = gpio_int_type;
    pin_cfg.mode = GPIO_MODE_INPUT;
    pin_cfg.pin_bit_mask = 1ULL << gpio_num;
    pin_cfg.pull_down_en = !is_pull_up;
    pin_cfg.pull_up_en = is_pull_up;
    gpio_config(&pin_cfg);

    gpio_isr_handler_add(gpio_num, isr_handler, arg);
}