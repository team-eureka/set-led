#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>
#include "set-led.h"

blink_step *parse_args (int argc, char **argv, bool *daemonize, bool *verbose);

#endif
