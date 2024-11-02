/*
    https://docs.espressif.com/projects/esp-idf/zh_CN/release-v5.1/esp32/api-reference/peripherals/ledc.html
 * LEDC Chan to Group/Channel/Timer Mapping
 ** ledc: 0  =>  Channel: 0, Timer: 0
 ** ledc: 1  =>  Channel: 1, Timer: 0
 ** ledc: 2  =>  Channel: 2, Timer: 1
 ** ledc: 3  =>  Channel: 3, Timer: 1
 ** ledc: 4  =>  Channel: 4, Timer: 2
 ** ledc: 5  =>  Channel: 5, Timer: 2
 ** ledc: 6  =>  Channel: 6, Timer: 3
 ** ledc: 7  =>  Channel: 7, Timer: 3
 */
/*  PWM的通道，共8个(0-15)，分为高低速两组，高速通道(0-7): 80MHz时钟，
 *                                        低速通道(0-7): 1MHz时钟。
 *  初始化ledc定时器(LEDC_TIM0 -- LEDC_TIM3) ==> 关联PIN  ==> 输出
 *
 */

#include "ledc.h"

void ledc_init(void)
{
    /* 配置定时器 */
    ledc_timer_config_t ledc_timer = {};
    ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_timer.duty_resolution = LEDC_TIMER_10_BIT;
    ledc_timer.timer_num = LEDC_TIMER_0;
    ledc_timer.freq_hz = 5000;
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&ledc_timer);
    /* 配置通道 */
    ledc_channel_config_t ledc_channel0 = {};
    ledc_channel0.channel = LEDC_CHANNEL_0,
    ledc_channel0.gpio_num = GPIO_NUM_2,
    ledc_channel0.hpoint = 0,
    ledc_channel0.duty = 0,
    // ledc_channel.intr_type =
    // ledc_channel.flags =
    ledc_channel0.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel0.timer_sel = LEDC_TIMER_0;
    ledc_channel_config(&ledc_channel0);

    ledc_channel_config_t ledc_channel1 = {};
    ledc_channel1.channel = LEDC_CHANNEL_1,
    ledc_channel1.gpio_num = GPIO_NUM_4,
    ledc_channel1.hpoint = 0,
    ledc_channel1.duty = 0,
    // ledc_channel.intr_type =
    // ledc_channel.flags =
    ledc_channel1.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel1.timer_sel = LEDC_TIMER_0;
    ledc_channel_config(&ledc_channel1);
}