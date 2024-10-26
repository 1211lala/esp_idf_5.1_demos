#ifndef _GPIO_INT_H_
#define _GPIO_INT_H_

#include "common.h"

void gpio_isr_init(gpio_num_t gpio_num, gpio_int_type_t gpio_int_type, bool is_pull_up, gpio_isr_t isr_handler, void *arg);

#endif