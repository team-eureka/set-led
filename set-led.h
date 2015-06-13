#ifndef SET_LED_H
#define SET_LED_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

// Macros

#define dbg_printf(...) \
	if (verbose) \
		printf(__VA_ARGS__)

// Config

#define NUM_LEDS 2
extern const int NUM_COLORS;

typedef struct {
	const char *color;
	const int gpio_pin;
	const int pwm_channel;

	bool uses_pwm;
	uint32_t termcnt;
	int pwm_dev;
} led;

typedef struct {
	const char *name;
	const bool led_states[NUM_LEDS];
} color;

extern led leds[];
extern const color colors[];

// Structures & enums

struct blink_step {
	int color_ind;
	time_t duration;
	struct blink_step *next;
};
typedef struct blink_step blink_step;

typedef enum {
	SOLID,
	BLINKING,
	INVALID,
} mode;

// Global state

extern bool verbose;

#endif
