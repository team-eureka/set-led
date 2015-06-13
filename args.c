#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <strings.h>

#include "args.h"

static mode mode_from_name (const char *name);
static int color_from_name (const char *name);
static blink_step *state_from_mode (int color, mode mode);

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

	char *colorstr;
	mode mode = SOLID;

	int num_args = (argc - optind);
	if (num_args == 1) {
		colorstr = argv[optind];
	} else if (num_args == 2) {
		colorstr = argv[optind];
		char *modestr = argv[optind + 1];

		mode = mode_from_name(modestr);
		if (mode == INVALID) {
			fprintf(stderr, "Unknown mode \"%s\"\n", modestr);
			return NULL;
		}
	} else {
		return NULL;
	}

	int color = color_from_name(colorstr);
	if (color == -1) {
		fprintf(stderr, "Unknown color \"%s\"\n", colorstr);
		return NULL;
	}


	return state_from_mode(color, mode);
}

static mode mode_from_name (const char *name) {
	if (strcasecmp(name, "solid") == 0) {
		return SOLID;
	} else if (strcasecmp(name, "blinking") == 0) {
		return BLINKING;
	} else {
		return INVALID;
	}
}

static int color_from_name (const char *name) {
	for (int i = 0; i < NUM_COLORS; ++i) {
		if (strcasecmp(name, colors[i].name) == 0)
			return i;
	}

	return -1;
}

static blink_step *state_from_mode (int color, mode mode) {
	blink_step *pattern = calloc(1, sizeof(blink_step));

	pattern->color_ind = color;

	if (mode == BLINKING) {
		pattern->duration = 1000; // TODO: Time units?

		pattern->next = calloc(1, sizeof(blink_step));
		pattern->next->color_ind = 0; // Off
		pattern->next->duration = 1000; // TODO: Ditto.
	}

	return pattern;
}
