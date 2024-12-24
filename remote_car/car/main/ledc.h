#ifndef _P_LEDC_H_
#define _P_UART_H_

#include "datafile.h"
#include "gpio.h"
#include "driver/ledc.h"

#ifdef USE_MOTORB
#define ENABLE_MOTOR_NUM 2
#else
#define ENABLE_MOTOR_NUM 1
#endif
enum MOTOR_DIR
{
    STOP = 0,
    RIGHT,
    LEFT,
};
#define MAXLIMIT 80 // 允许的最大占空比
struct Motor
{
    uint8_t dir;  // 电机方向 STOP、LETF、RIGHT
    uint8_t duty; // 电机占空比 0 - 100

    int8_t speed;
    int8_t angle;

    uint8_t pwmPin;
    uint8_t in1Pin;
    uint8_t in2Pin;
    uint8_t chaneel;
};
extern struct Motor Motor[];

#define MotorA Motor[0]
#define MotorB Motor[1]

void motor_cfg(struct Motor *motor);
void motor_set_params(struct Motor *motor);

void task_ledc(void *arg);

void beep_cfg(void);
void beep_set(uint16_t freq, uint16_t duty);

#endif