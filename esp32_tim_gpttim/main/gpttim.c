/**
 * https://docs.espressif.com/projects/esp-idf/zh_CN/v5.1.3/esp32/api-reference/peripherals/gptimer.html
 * 通用定时器实例由 gptimer_handle_t 表示,后台驱动会在资源池中管理所有可用的硬件资源，不需要考虑硬件所属的定时器以及定时器组
 */

#include "gpttim.h"
/******************************************************************************
 * 函数描述: gpttim初始化，设置定时器计数频率为1M，可自动重加载值为0，向上计数
 * 参  数1: 定时中断回调函数 bool (*gptimer_alarm_cb_t) (gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx);
 * 参  数2: 定时器计数频率
 * 参  数3: 定时器向上计数终值
 *******************************************************************************/
void gpttim_init(struct GPTTIM_PARAM *gpt_param)
{
    /* 1:配置GPT参数 */
    gptimer_config_t timer_config = {};
    timer_config.clk_src = GPTIMER_CLK_SRC_APB;
    timer_config.direction = GPTIMER_COUNT_UP;
    // timer_config.flags.intr_shared = 0;
    timer_config.resolution_hz = gpt_param->freq;
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gpt_param->gptimer));
    /* 2:配置GPT回调函数 */
    gptimer_event_callbacks_t cbs = {};
    cbs.on_alarm = gpt_param->gpttimer_cb;
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gpt_param->gptimer, &cbs, gpt_param->arg));

    /* 4:配置闹钟事件动作 */
    gptimer_alarm_config_t alarm_config = {};
    alarm_config.flags.auto_reload_on_alarm = true;
    alarm_config.alarm_count = gpt_param->target_cnt;
    alarm_config.reload_count = 0;
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gpt_param->gptimer, &alarm_config));
    /* 5:启动定时器 */
}

/* 开启定时器 */
void gpttim_start(struct GPTTIM_PARAM *gpt_param)
{
    ESP_ERROR_CHECK(gptimer_enable(gpt_param->gptimer));
    ESP_ERROR_CHECK(gptimer_start(gpt_param->gptimer));
}
/* 关闭定时器 */
void gpttim_stop(struct GPTTIM_PARAM *gpt_param)
{
    ESP_ERROR_CHECK(gptimer_stop(gpt_param->gptimer));
    ESP_ERROR_CHECK(gptimer_disable(gpt_param->gptimer));
}
/* 复位定时器, 未测试 */
void gpttim_reset(struct GPTTIM_PARAM *gpt_param)
{
    ESP_ERROR_CHECK(gptimer_stop(gpt_param->gptimer));
    ESP_ERROR_CHECK(gptimer_disable(gpt_param->gptimer));
    ESP_ERROR_CHECK(gptimer_del_timer(gpt_param->gptimer));
    memset(gpt_param, 0x00, sizeof(struct GPTTIM_PARAM));
}