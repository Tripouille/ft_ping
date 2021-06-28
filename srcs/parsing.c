#include "parsing.h"

void
parse_arguments(char ** args) {
	t_option *		option;

	for (int it = 0; args[it]; ++it) {
		if (args[it][0] == '-') {
			if (!args[it][1]) usage();
			for (int is = 1; args[it][is]; ++is) {
				option = get_option(g_ping.options, args[it][is]);
				if (option == NULL) usage();
				option->active = true;
				if (option->require_value) {
					if (args[it][is + 1]) option->value = args[it] + is + 1;
					else if (args[it + 1]) option->value = args[++it];
					else usage();
					break ;
				}
			}
		}
		else if (g_ping.host == NULL) g_ping.host = args[it];
		else usage();
	}
}

void
usage(void) {
	char const *	message = "usage: ping [-vh] [-s <size>] [-t ttl] host";

	fprintf(stderr, "%s\n", message);
	exit(EXIT_FAILURE);
}