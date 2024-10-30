#ifndef _GPTTIM_H_
#define _GPTTIM_H_

#include "common.h"
#include "driver/gptimer.h"

struct GPTTIM_PARAM
{
    gptimer_handle_t gptimer;       /* gpttimer 句柄 */
    uint8_t id;                     /* 记录定时器ID */
    uint32_t freq;                  /* 设置定时器频率 */
    uint32_t target_cnt;            /* 设置定时器计数值 */
    gptimer_alarm_cb_t gpttimer_cb; /* 设置定时器回调 */
    void *arg;                      /* 回调参数 */
};

void gpttim_init(struct GPTTIM_PARAM *gpt_param);
void gpttim_start(struct GPTTIM_PARAM *gpt_param);
#endif