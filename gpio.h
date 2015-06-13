#ifndef GPIO_H
#define GPIO_H

#include "set-led.h"

int init_gpio (void);
void gpio_set_brightness (const led *l, int percent);

#endif
