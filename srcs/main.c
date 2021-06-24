#include "ping.h"

t_ping_infos	g_ping_infos;

static void
signal_handler(int signal) {
	(void)signal;
	g_ping_infos.active = false;
	exit(EXIT_SUCCESS);
}

static void
usage(void) {
	char const *	message = "usage: ping [-vh] host";

	fprintf(stderr, "%s\n", message);
	exit(EXIT_FAILURE);
}

static void
parse_arguments(char ** args) {
	t_option *		option;

	for (int it = 0; args[it]; ++it) {
		if (args[it][0] == '-') {
			if (!args[it][1])
				usage();
			for (int is = 1; args[it][is]; ++is) {
				option = get_option(g_ping_infos.options, args[it][is]);
				if (option == NULL)
					usage();
				option->active = true;
				if (option->require_value) {
					if (args[it][is + 1])
						option->value = args[it] + is + 1;
					else if (args[it + 1])
						option->value = args[++it];
					else
						usage();
					break ;
				}
			}
		}
		else if (g_ping_infos.host == NULL)
			g_ping_infos.host = args[it];
		else
			usage();
	}
}

static void
print_options(void) {
	for (int i = 0; i < OPTION_NUMBER; ++i) {
		printf("option %c: active = %i, value = %s\n", g_ping_infos.options[i].id, g_ping_infos.options[i].active, g_ping_infos.options[i].value);
	}
}

int
main(int ac, char ** av) {
	(void)ac;
	g_ping_infos.host = NULL;
	g_ping_infos.active = true;
	initialize_options(g_ping_infos.options);
	parse_arguments(av + 1);
	if (g_ping_infos.host == NULL)
		usage();
	else if (get_option(g_ping_infos.options, 'h')->active)
		usage();

	// int socket_fd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
    // if (socket_fd == -1)
	// 	print_error_exit("Socket file descriptor not received");
  
    signal(SIGINT, signal_handler);
	print_options();
	printf("host %s\n", g_ping_infos.host);

	return (0);
}

/*
◦ getpid.
◦ getuid.
◦ getaddrinfo.
◦ gettimeofday.
◦ inet_ntop.
◦ inet_pton
◦ exit.
◦ signal.
◦ alarm.
◦ setsockopt.
◦ recvmsg.
◦ sendto.
◦ socket.
◦ les fonctions de la famille printf.
◦ Vous avez l’autorisation d’utiliser d’autres fonctions dans le cadre de vos bonus,
à condition que leur utilisation soit dûment justifiée lors de votre correction.
Soyez malins.
*/