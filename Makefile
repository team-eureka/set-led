OBJECTS = set-led.o config.o args.o gpio.o pwm.o
CFLAGS = -Wall -Wextra -std=c99

all: set-led
set-led: $(OBJECTS)

clean:
	-rm -f set-led *.o

.PHONY: clean
