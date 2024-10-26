#include "gpio.h"
#include "common.h"

/******************************************************************************
 * 函数描述: ESP32的GPIO2初始化为输出的方式一
 * gpio_num_t: GPIO_NUM_X
 * gpio_mode_t: GPIO_MODE_DISABLE
 *              GPIO_MODE_INPUT
 *              GPIO_MODE_OUTPUT
 *              GPIO_MODE_OUTPUT_OD
 *              GPIO_MODE_INPUT_OUTPUT_OD
 *              GPIO_MODE_INPUT_OUTPUT
 *******************************************************************************/
void gpio_fast_init(gpio_num_t gpio_num, gpio_mode_t gpio_mode)
{
    gpio_reset_pin(gpio_num);
    gpio_set_direction(gpio_num, gpio_mode);
}
/******************************************************************************
 * 函数描述: ESP32的GPIO2初始化为输出的方式二
 * gpio_num_t: GPIO_NUM_X
 * gpio_mode_t: GPIO_MODE_DISABLE
 *              GPIO_MODE_INPUT
 *              GPIO_MODE_OUTPUT
 *              GPIO_MODE_OUTPUT_OD
 *              GPIO_MODE_INPUT_OUTPUT_OD
 *              GPIO_MODE_INPUT_OUTPUT
 * is_pull_up: 是否上拉
 *******************************************************************************/
void gpio_general_init(gpio_num_t gpio_num, gpio_mode_t gpio_mode, bool is_pull_up)
{
    gpio_config_t pin_cfg = {};

    pin_cfg.intr_type = GPIO_INTR_DISABLE;
    pin_cfg.mode = gpio_mode;
    pin_cfg.pull_down_en = !is_pull_up;
    pin_cfg.pull_up_en = is_pull_up;
    pin_cfg.pin_bit_mask = (1ULL << gpio_num);
    gpio_config(&pin_cfg);
}
/******************************************************************************
 * 函数描述: GPIO输出状态翻转
 * GPIO只有在使用 GPIO_MODE_INPUT_OUTPUT 模式时才可以翻转
 *******************************************************************************/
void gpio_tiggle(gpio_num_t gpio_num)
{
    gpio_set_level(gpio_num, !gpio_get_level(gpio_num));
}

/******************************************************************************************************************************/
uint8_t get_key_value(uint8_t pin)
{
    static uint8_t flag = 1;

    if (gpio_get_level(pin) != false)
    {
        flag = 1;
    }
    if (flag == 1 && gpio_get_level(pin) == false)
    {
        flag = 0;
        return true;
    }
    return false;
}