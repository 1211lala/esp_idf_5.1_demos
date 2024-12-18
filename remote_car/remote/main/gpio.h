#ifndef _GPIO_H_
#define _GPIO_H_

#include "datafile.h"
void gpio_fast_init(gpio_num_t gpio_num, gpio_mode_t gpio_mode);
void gpio_general_init(gpio_num_t gpio_num, gpio_mode_t gpio_mode, bool is_pull_up);
void gpio_tiggle(gpio_num_t gpio_num);

#endif