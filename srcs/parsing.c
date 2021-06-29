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
	char const *	message =	"Usage\n"
									"  ping [options] <destination>\n"
								"Options:\n"
									"  -c <count>         stop after <count> replies\n"
									"  -h                 print help and exit\n"
									"  -i <interval>      seconds between sending each packet\n"
									"  -n                 no dns name resolution\n"
									"  -q                 quiet output\n"
									"  -s <size>          use <size> as number of data bytes to be sent\n"
									"  -t <ttl>           define time to live\n"
									"  -v                 Verbose output.  ICMP packets other than ECHO_RESPONSE that are received are listed\n";

	fprintf(stderr, "%s\n", message);
	exit(EXIT_FAILURE);
}