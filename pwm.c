#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "api/pwm.h"
#include "api/kgpio.h" // For pinmux

#include "pwm.h"

static int enable_pwm (const led *l);

static unsigned int pwm_reg_read (const led *l, unsigned int offset);
static void pwm_reg_write (const led *l, unsigned int offset, unsigned int data);

int init_pwm (led *l) {
	l->uses_pwm = false;

	char pwm_path[20];
	snprintf(pwm_path, 20, "/dev/pwmch%d", l->pwm_channel);

	l->pwm_dev = open(pwm_path, O_RDONLY);
	if (l->pwm_dev == -1) {
		dbg_printf("PWM not available for %s LED, using GPIO\n", l->color);
		return -1;
	}

	if (!pwm_reg_read(l, ENABLE_REG_OFFSET)) {
		dbg_printf("PWM not enabled for %s LED, attempting to enable\n", l->color);
		if (enable_pwm(l) == -1) {
			close(l->pwm_dev);
			l->pwm_dev = -1;
			return -1;
		}
	}

	l->uses_pwm = true;
	l->termcnt = pwm_reg_read(l, TERMCNT_REG_OFFSET);

	dbg_printf("PWM enabled for %s LED with tcnt %d, using\n", l->color, l->termcnt);

	return 0;
}

static int enable_pwm (const led *l) {
	int gpio_dev;
	if ((gpio_dev = open("/dev/gpio", O_RDONLY)) != -1) {
		if (ioctl(gpio_dev, GPIO_IOCTL_ATTACH_PWM, 1) == -1) {
			dbg_printf("pinmux ioctl not available, falling back to GPIO (are you using a stock kernel?)\n");
			return -1;
		}

		close(gpio_dev);
	} else {
		dbg_printf("Failed to open /dev/gpio to set pinmux. Attempting PWM, but expect trouble...\n");
	}

	unsigned int control = (DEFAULT_PRESCALE & 7) | ((DEFAULT_POLARITY & 1) << 3);

	pwm_reg_write(l, CONTROL_REG_OFFSET, control);
	pwm_reg_write(l, TERMCNT_REG_OFFSET, DEFAULT_TERMCNT);
	pwm_reg_write(l, ENABLE_REG_OFFSET, 1);

	return 0;
}

void pwm_set_brightness (const led *l, int percent) {
	unsigned int duty = (percent * l->termcnt) / 100;
	pwm_reg_write(l, DUTY_REG_OFFSET, duty);
}

static unsigned int pwm_reg_read (const led *l, unsigned int offset) {
	galois_pwm_rw_t state = { .offset = offset };

	if (ioctl(l->pwm_dev, PWM_IOCTL_READ, &state) == -1) {
		perror("Failed to read from PWM register");
		exit(EXIT_FAILURE);
	}

	return state.data;
}

static void pwm_reg_write (const led *l, unsigned int offset, unsigned int data) {
	galois_pwm_rw_t state = { .offset = offset, .data = data };

	if (ioctl(l->pwm_dev, PWM_IOCTL_WRITE, &state) == -1) {
		perror("Failed to write to PWM register");
		exit(EXIT_FAILURE);
	}
}
