#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <getopt.h>
#include "kgpio.h"

const int led_ports[] = {
	17, // White
	18  // Red
};
#define NUM_LEDS sizeof(led_ports) / sizeof(int)

// Structures
typedef struct {
	const char *name;
	bool led_states[NUM_LEDS];
} color;

struct blink_step {
	int color_ind;
	time_t duration;
	struct blink_step *next;
};
typedef struct blink_step blink_step;

// Constants
const color colors[] = {
	{"off",   {false, false}},
	{"white", {true,  false}},
	{"red",   {false, true }}
};
#define NUM_COLORS sizeof(colors) / sizeof(color)

bool verbose = false;
int gpio_dev = -1;

// Functions
blink_step *parse_args (int argc, char **argv, bool *daemonize, bool *verbose);
void print_usage (char *name);
void set_state (const blink_step *s);
void set_color (int color_ind);

int main (int argc, char **argv) {
	bool daemonize = false;
	blink_step *target = parse_args(argc, argv, &daemonize, &verbose);
	if (target == NULL) {
		print_usage(argv[0]);
	}

	if ((gpio_dev = open("/dev/gpio", O_RDONLY)) == -1) {
		perror("Failed to open /dev/gpio");
		exit(EXIT_FAILURE);
	}

	set_state(target);

	return EXIT_SUCCESS;
}

blink_step *parse_args (int argc, char **argv, bool *daemonize, bool *verbose) {
	static struct option opts[] = {
		{"daemonize", no_argument, NULL, 'd'},
		{"verbose",   no_argument, NULL, 'v'}
	};

	int c = 0;
	while ((c = getopt_long(argc, argv, "dv", opts, NULL)) != -1) {
		switch (c) {
			case 'd':
				*daemonize = true;
				break;
			case 'v':
				*verbose = true;
				break;
			default:
				return NULL;
		}
	}

	char *color = "off";
	bool blinking = false;

	int num_args = (argc - optind);
	if (num_args == 1) {
		color = argv[optind];
	} else if (num_args == 2) {
		color = argv[optind];

		if (strcasecmp(argv[optind + 1], "solid") == 0) {
			blinking = false;
		} else if (strcasecmp(argv[optind + 1], "blinking") == 0) {
			blinking = true;
		} else {
			return NULL;
		}
	} else {
		return NULL;
	}

	blink_step *main = calloc(1, sizeof(blink_step));
	int i;
	for (i = 0; i < NUM_COLORS; ++i) {
		if (strcasecmp(color, colors[i].name) == 0) {
			main->color_ind = i;
			break;
		}
	}
	if (i == NUM_COLORS) {
		fprintf(stderr, "Unknown color \"%s\"\n", color);
		free(main);
		return NULL;
	}

	if (blinking) {
		main->duration = 1000; // TODO: Time units?
		main->next = calloc(1, sizeof(blink_step));
		main->next->color_ind = 0;
		main->next->duration = 1000; // TODO: Ditto.
	}

	return main;
}

void print_usage (char *name) {
	fprintf(stderr, "Usage: %s [-d|--daemonize] [-v|--verbose] white|red|off [solid|blinking]\n", name);
	exit(EXIT_FAILURE);
}

void set_state (const blink_step *s) {
	set_color(s->color_ind);
}

void set_color (int color_ind) {
	const color *c = &colors[color_ind];

	galois_gpio_data_t port_state = {0};
	port_state.mode = GPIO_MODE_DATA_OUT;

	int i;
	for (i = 0; i < NUM_LEDS; ++i) {
		port_state.port = led_ports[i];
		port_state.data = c->led_states[i] ? 1 : 0;

		if (ioctl(gpio_dev, GPIO_IOCTL_SET, &port_state) == -1) {
			perror("Failed to set LED direction");
		}
		if (ioctl(gpio_dev, GPIO_IOCTL_WRITE, &port_state) == -1) {
			perror("Failed to set LED state");
		}
	}
}
