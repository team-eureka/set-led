EXECUTABLE = set-led
OBJECTS = set-led.o config.o args.o gpio.o pwm.o

CFLAGS = -Wall -Wextra -std=c99

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)

clean:
	-rm -f $(EXECUTABLE) $(OBJECTS)

.PHONY: clean
