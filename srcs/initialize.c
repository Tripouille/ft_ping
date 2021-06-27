#include "initialize.h"

static void
print_options(void) {
	for (int i = 0; i < OPTION_NUMBER; ++i) {
		printf("option %c: active = %i, value = %s\n", g_ping.options[i].id, g_ping.options[i].active, g_ping.options[i].value);
	}
}

static void
signal_handler(int signal) {
	(void)signal;
	struct timeval	now;

	gettimeofday(&now, NULL);
    printf("\n--- %s ping statistics ---\n", g_ping.host);
	printf("%li packets transmitted, %li received, %g%% packet loss, time %.fms\n",
		g_ping.msg_count, g_ping.msg_received_count, 100.0 - (g_ping.msg_received_count / (double)g_ping.msg_count * 100),
		get_elapsed_us(&g_ping.start, &now) / 1E3);
	if (g_ping.msg_received_count) {
		printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms",
			g_ping.min, g_ping.total / g_ping.msg_received_count, g_ping.max, 0.42); //faire la liste chaine
	}
	free(g_ping.sent_packet);
	exit(EXIT_SUCCESS);
}

static void
apply_options(void) {
	t_option *		option;

	if ((option = get_option(g_ping.options, 's'))->active) {
		g_ping.packet_msg_size = parse_int(option->value, 0, INT_MAX);
	}
}


void
initialize_config(char ** av) {
	mset(&g_ping, sizeof(g_ping), 0);
	g_ping.packet_msg_size = 56;
	g_ping.min = -1;
	initialize_options(g_ping.options);
	parse_arguments(av + 1);
	apply_options();
	if (g_ping.host == NULL || get_option(g_ping.options, 'h')->active) usage();
	g_ping.socket_fd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (g_ping.socket_fd == -1) print_error_exit("Socket file descriptor not received");
    g_ping.pid = getpid();
    g_ping.msg_count = 1;
	signal(SIGINT, signal_handler);
	print_options();
}

void
initialize_socket(void) {
    struct timeval	timeout = {0, 1E3};
	int				ttl = TTL;

    setsockopt(g_ping.socket_fd, SOL_IP, IP_TTL, &ttl, sizeof(ttl));
    setsockopt(g_ping.socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}


void
initialize_packet(void * packet, size_t packet_size) {
	struct icmphdr *	header = packet;

    mset(packet, packet_size, 0);
    header->type = ICMP_ECHO;
    header->un.echo.id = g_ping.pid;
    header->un.echo.sequence = g_ping.msg_count;
    header->checksum = checksum(packet, packet_size);
}

void
initialize_msg(struct msghdr * msg, struct iovec * iov) {
	mset(msg, sizeof(*msg), 0);
	msg->msg_iov = iov;
	msg->msg_iovlen = 1;
}