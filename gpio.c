#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "api/kgpio.h"

#include "gpio.h"

static int gpio_dev = -1;

static void gpio_set_led (const led *l, bool on);

int init_gpio (void) {
	if ((gpio_dev = open("/dev/gpio", O_RDONLY)) == -1) {
		perror("Failed to open /dev/gpio");
		return -1;
	}

	return 0;
}

void gpio_set_brightness (const led *l, int percent) {
	if (percent >= 10)
		gpio_set_led(l, true);
	else
		gpio_set_led(l, false);
}

static void gpio_set_led (const led *l, bool on) {
	galois_gpio_data_t port_state = {0};

	port_state.mode = GPIO_MODE_DATA_OUT;
	port_state.port = l->gpio_pin;
	port_state.data = on ? 1 : 0;

	if (ioctl(gpio_dev, GPIO_IOCTL_SET, &port_state) == -1) {
		perror("Failed to set GPIO direction");
		exit(EXIT_FAILURE);
	}

	if (ioctl(gpio_dev, GPIO_IOCTL_WRITE, &port_state) == -1) {
		perror("Failed to set GPIO state");
		exit(EXIT_FAILURE);
	}
}
