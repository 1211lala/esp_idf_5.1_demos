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

struct Motor Motor[2] = {
    {
        // 电机A
        .chaneel = LEDC_CHANNEL_0,
        .dir = STOP,
        .duty = 0,
        .in1Pin = AIN1,
        .in2Pin = AIN2,
        .pwmPin = AINPWM,
        .speed = 0,
        .angle = 0,
    },
#ifdef USE_MOTORB
    {
        // 电机B
        .chaneel = LEDC_CHANNEL_1,
        .dir = STOP,
        .duty = 0,
        .in1Pin = BIN1,
        .in2Pin = BIN2,
        .pwmPin = BINPWM,
    },
#endif
};

void motor_pwm_cfg(uint16_t freq)
{
    /* 配置定时器 LEDC_TIMER_0 */
    ledc_timer_config_t ledc_timer = {};
    ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_timer.duty_resolution = LEDC_TIMER_10_BIT;
    ledc_timer.timer_num = LEDC_TIMER_0;
    ledc_timer.freq_hz = freq;
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&ledc_timer);

    /* 配置通道 LEDC_TIMER_0 ->LEDC_CHANNEL_0 MotorA */
    ledc_channel_config_t ledc_channel0 = {};
    ledc_channel0.channel = MotorA.chaneel;
    ledc_channel0.gpio_num = MotorA.pwmPin;
    ledc_channel0.hpoint = 0;
    ledc_channel0.duty = 0;
    ledc_channel0.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel0.timer_sel = LEDC_TIMER_0;
    ledc_channel_config(&ledc_channel0);

#ifdef USE_MOTORB
    /* 配置通道 LEDC_TIMER_0 ->LEDC_CHANNEL_1 MotorB */
    ledc_channel_config_t ledc_channel1 = {};
    ledc_channel1.channel = MotorB.chaneel,
    ledc_channel1.gpio_num = MotorB.pwmPin,
    ledc_channel1.hpoint = 0;
    ledc_channel1.duty = 0;
    ledc_channel1.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel1.timer_sel = LEDC_TIMER_0;
    ledc_channel_config(&ledc_channel1);
#endif
}

/****************************************************************************
 * 电机初始化 方向控制引脚, PWM引脚, 电机停止转动
 */
void motor_cfg(struct Motor *motor)
{
    /* 设置电机PWM频率为1000hz */
    motor_pwm_cfg(1000);
    /* 初始化电机方向引脚 */
    for (int i = 0; i < ENABLE_MOTOR_NUM; i++)
    {
        gpio_general_init((motor + i)->in1Pin, GPIO_MODE_INPUT_OUTPUT, true);
        gpio_general_init((motor + i)->in2Pin, GPIO_MODE_INPUT_OUTPUT, true);
        gpio_set_level((motor + i)->in1Pin, false);
        gpio_set_level((motor + i)->in2Pin, false);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, (motor + i)->chaneel, (motor + i)->duty);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, (motor + i)->chaneel);
    }
}
/****************************************************************************
 * 功  能:设置电机参数保存在内存中
 * 参  数: 需要设置的电机结构体参数 &MotorA &MotorB
 */
void motor_set_params(struct Motor *motor)
{
#ifndef USE_MOTORB
    if (motor == &MotorB)
    {
        ESP_LOGE("motor_set_params()", "MotorB 未使能 ---> #define USE_MOTORB ");
        return;
    }
#endif
    switch (motor->dir)
    {
    case STOP:
        gpio_set_level(motor->in1Pin, false);
        gpio_set_level(motor->in2Pin, false);
        break;
    case RIGHT:
        gpio_set_level(motor->in1Pin, true);
        gpio_set_level(motor->in2Pin, false);
        break;
    case LEFT:
        gpio_set_level(motor->in1Pin, false);
        gpio_set_level(motor->in2Pin, true);
        break;
    default:
        gpio_set_level(motor->in1Pin, false);
        gpio_set_level(motor->in2Pin, false);
        break;
    }
    if (motor->duty > MAXLIMIT)
        motor->duty = MAXLIMIT;

    ledc_set_duty(LEDC_HIGH_SPEED_MODE, (motor)->chaneel, (uint32_t)motor->duty * 10.24);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, (motor)->chaneel);
}

void server_cfg(void)
{
    /* 配置定时器 LEDC_TIMER_1 */
    ledc_timer_config_t ledc_timer = {};
    ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_timer.duty_resolution = LEDC_TIMER_10_BIT;
    ledc_timer.timer_num = LEDC_TIMER_1;
    ledc_timer.freq_hz = 50;
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&ledc_timer);

    /* 配置通道 LEDC_TIMER_1 ->LEDC_CHANNEL_2*/
    ledc_channel_config_t ledc_channel = {};
    ledc_channel.channel = LEDC_CHANNEL_2;
    ledc_channel.gpio_num = SERVER;
    ledc_channel.hpoint = 0;
    ledc_channel.duty = 0;
    ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel.timer_sel = LEDC_TIMER_1;
    ledc_channel_config(&ledc_channel);
}

/**
 *
 * 一个周期 20ms   50hz
 * 20ms / 1024 = 0.0195ms
    0度-----------0.5ms   25
    45度----------1ms     50
    90度-----------1.5ms  75
    135度----------2.0ms  100
    180度-----------2.5ms 125

    100 / 180 = 0.5555;
 */
void server_angle(int8_t angle)
{
    int8_t temp = 0;

    angle = -angle;

    if (angle > 75)
        angle = 75;
    if (angle < -75)
        angle = -75;

    temp = 75 + 0.5555 * angle;
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, temp);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
}

void beep_cfg(void)
{
    /* 配置定时器 LEDC_TIMER_2 */
    ledc_timer_config_t ledc_timer = {};
    ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_timer.duty_resolution = LEDC_TIMER_10_BIT;
    ledc_timer.timer_num = LEDC_TIMER_2;
    ledc_timer.freq_hz = 4000;
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&ledc_timer);

    /* 配置通道 LEDC_TIMER_1 ->LEDC_CHANNEL_2*/
    ledc_channel_config_t ledc_channel = {};
    ledc_channel.channel = LEDC_CHANNEL_4;
    ledc_channel.gpio_num = BEEP;
    ledc_channel.hpoint = 0;
    ledc_channel.duty = 0;
    ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel.timer_sel = LEDC_TIMER_2;
    ledc_channel_config(&ledc_channel);
}

void beep_set(uint16_t freq, uint16_t duty)
{
    ledc_set_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_2, freq);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_2, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_4);
}
/****************************************************************************************************************** */
void task_ledc(void *arg)
{
    motor_cfg(Motor);
    server_cfg();
    server_angle(0);
    uint32_t val = 0;
    while (1)
    {
        if (pdPASS == xTaskNotifyWait(0x0000000, 0x00000000, &val, portMAX_DELAY))
        {
            if (MotorA.speed > 0)
            {
                MotorA.dir = RIGHT;
            }
            else if (MotorA.speed < 0)
            {
                MotorA.dir = LEFT;
            }
            else
            {
                MotorA.dir = STOP;
            }
            MotorA.duty = abs(MotorA.speed);

            motor_set_params(&MotorA);
            server_angle(MotorA.angle);

            printf("MotorA.speed: %d MotorA.angle: %d\n", MotorA.speed, MotorA.angle);
        }
    }
}