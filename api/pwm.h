#ifndef __GALOIS_PWM_HEADER__
#define __GALOIS_PWM_HEADER__

#define GALOIS_PWMCH_NUM	4

#define ENABLE_REG_OFFSET	0x0
#define CONTROL_REG_OFFSET	0x4
#define DUTY_REG_OFFSET		0x8
#define TERMCNT_REG_OFFSET	0xC

/*
 * ioctl command
 */
#define PWM_IOCTL_READ  0x1234
#define PWM_IOCTL_WRITE 0x4567

typedef struct galois_pwm_rw {
	unsigned int offset;	/* the offset of register in PWM */
	unsigned int data;	/* the value will be read/write from/to register */
} galois_pwm_rw_t;

#endif
