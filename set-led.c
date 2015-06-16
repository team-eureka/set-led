#include <stdio.h>
#include <stdlib.h>

#include "set-led.h"
#include "args.h"
#include "gpio.h"
#include "pwm.h"

bool verbose = false;

static void print_usage (char *name);
static void set_state (const blink_step *s);
static void set_color (const color *c);
static void set_led (const led *l, bool on);
static int init_leds(void);

int main (int argc, char **argv) {
	bool daemonize = false;

	blink_step *target = parse_args(argc, argv, &daemonize, &verbose);
	if (target == NULL) {
		print_usage(argv[0]);
	}

	if (init_leds() == -1) {
		fprintf(stderr, "Failed to initialize LEDs\n");
		exit(EXIT_FAILURE);
	}

	set_state(target);

	return EXIT_SUCCESS;
}


static void print_usage (char *name) {
	fprintf(stderr, "Usage: %s [-v|--verbose] white|red|off [solid|blinking]\n", name);
	exit(EXIT_FAILURE);
}

static void set_state (const blink_step *s) {
	set_color(&colors[s->color_ind]);
}

static void set_color (const color *c) {
	for (int i = 0; i < NUM_LEDS; ++i)
		set_led(&leds[i], c->led_states[i]);
}

static void set_led (const led *l, bool on) {
	dbg_printf("Turning %s %s LED\n", on ? "on" : "off", l->color);

	unsigned int brightness = on ? 10 : 0;
	if (l->uses_pwm)
		pwm_set_brightness(l, brightness);
	else
		gpio_set_brightness(l, brightness);

}

static int init_leds (void) {
	bool gpio_needed = false;

	for (int i = 0; i < NUM_LEDS; ++i) {
		if (init_pwm(&leds[i]) == -1) {
			gpio_needed = true;
		}
	}

	if (gpio_needed)
		return init_gpio();

	return 0;
}
