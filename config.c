#include "set-led.h"

#define LED(color, gpio_pin, pwm_channel) \
	{color, gpio_pin, pwm_channel, false, 0, -1}

// If you add another LED, make sure to update NUM_LEDS in set-led.h.
led leds[NUM_LEDS] = {
	LED("white", 17, 0),
	LED("red",   18, 1),
};

const color colors[] = {
	{"off",   {false, false}},
	{"white", {true,  false}},
	{"red",   {false, true }},
};

const int NUM_COLORS = sizeof(colors) / sizeof(colors[0]);
