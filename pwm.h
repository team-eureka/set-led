#ifndef PWM_H
#define PWM_H

#include "set-led.h"

#define DEFAULT_TERMCNT 75

#define DEFAULT_PRESCALE 1
#define DEFAULT_POLARITY 0

int init_pwm (led *l);
void pwm_set_brightness (const led *l, int percent);

#endif
