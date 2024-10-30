#ifndef _P_TIM_H_
#define _P_TIM_H_

#include "common.h"

#include "driver/timer.h"
#include "soc/rtc.h"

struct TIM_PARAM
{
    uint8_t id[2];                  /* 指定定时器 */
    uint32_t target_cnt;            /* 指定计数值 */
    uint32_t freq;                  /* 指定计数频率 10的倍数 */
    timer_isr_t timer_isr_callback; /* 指定回调函数 */
    void *arg;                      /* 指定参数 */
};

void tim_init(struct TIM_PARAM *tim_para);

#endif