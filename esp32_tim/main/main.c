/*
我的理解
    freertos在执行任务A时被中断打断，在中断中又通过队列启动了B任务，
    如果 pxHigherPriorityTaskWoken 为 true 则直接进入任务B，
    如果 pxHigherPriorityTaskWoken 为 false 则是先进入任务A，
    再判断是否有高于A的任务，进行任务切换


bool timer_callback(void *arg)
{
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;

    uint64_t value = timer_group_get_counter_value_in_isr(0, 0);
    xQueueSendFromISR(tim_queue, &value, &pxHigherPriorityTaskWoken);
    return pxHigherPriorityTaskWoken;
}
返回true、pdTRUE等定义为 1 的宏，或直接返回1，意味着ISR程序结束时发生一次上下文切换
返回false、pdFALSE等定义为 0 的宏，或直接返回0，意味着ISR程序结束后不发生上下文切换

它的第三个参数名为pxHigherPriorityTaskWoken，传入一个BaseType_t类型的指针。
作用是如果宏定义函数xQueueSendFromISR()造成了更高优先级任务进入就绪状态，则将这个指针指向的变量赋值为pdTRUE，否则则不发生改变。
你可以选择根据这个变量决定有没有必要发生一次上下文切换。
对于此callback函数，直接返回这个值即可。如果你在任何情况下，ISR程序结束时都不想发生上下文切换，
则一直返回 false 或者FreeRTOS风格的 pdFALSE即可
*/

#include "common.h"

#include "tim.h"
TaskHandle_t task_tim_handle = NULL;
QueueHandle_t tim00_isr_queue;
QueueHandle_t tim11_isr_queue;
uint64_t cnt00 = 0;
uint64_t cnt11 = 0;
bool timer_callback(void *arg)
{
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    struct TIM_PARAM *t = arg;
    if (t->id[0] == 0 && t->id[1] == 0)
    {
        cnt00++;
        xQueueSendFromISR(tim00_isr_queue, &cnt00, &pxHigherPriorityTaskWoken);
    }
    else if (t->id[0] == 1 && t->id[1] == 1)
    {
        cnt11++;
        xQueueSendFromISR(tim11_isr_queue, &cnt11, &pxHigherPriorityTaskWoken);
    }
    return pxHigherPriorityTaskWoken;
}

void IRAM_ATTR task_tim00(void *arg)
{
    struct TIM_PARAM *t = arg;
    timer_start(t->id[0], t->id[1]);
    while (1)
    {
        uint64_t value = 0;
        if (xQueueReceive(tim00_isr_queue, &value, portMAX_DELAY))
        {
            ESP_LOGI("task_tim00", "tim[%d][%d] value : %llu", t->id[0], t->id[1], value);
        }
    }
}
void IRAM_ATTR task_tim11(void *arg)
{
    struct TIM_PARAM *t = arg;
    timer_start(t->id[0], t->id[1]);
    while (1)
    {
        uint64_t value = 0;
        if (xQueueReceive(tim11_isr_queue, &value, portMAX_DELAY))
        {
            ESP_LOGI("task_tim11", "tim[%d][%d] value : %llu", t->id[0], t->id[1], value);
        }
    }
}
struct TIM_PARAM tim_00 = {

    .arg = &tim_00,
    .freq = 1000000,
    .id[0] = 0,
    .id[1] = 0,
    .target_cnt = 1000000,
    .timer_isr_callback = timer_callback,
};
struct TIM_PARAM tim_11 = {

    .arg = &tim_11,
    .freq = 1000000,
    .id[0] = 1,
    .id[1] = 1,
    .target_cnt = 100000,
    .timer_isr_callback = timer_callback,
};
void app_main(void)
{
    tim_init(&tim_00);
    tim_init(&tim_11);
    tim00_isr_queue = xQueueCreate(4, sizeof(uint64_t));
    tim11_isr_queue = xQueueCreate(4, sizeof(uint64_t));
    if (tim00_isr_queue == NULL)
    {
        ESP_LOGE("app_main", "tim00_isr_queue init fail.");
    }
    if (tim11_isr_queue == NULL)
    {
        ESP_LOGE("app_main", "tim11_isr_queue init fail.");
    }
    xTaskCreatePinnedToCore(task_tim00, "task_tim00", 1024 * 4, (void *)&tim_00, 5, NULL, 1);
    xTaskCreatePinnedToCore(task_tim11, "task_tim11", 1024 * 4, (void *)&tim_11, 5, NULL, 1);
    vTaskDelete(NULL);
}
