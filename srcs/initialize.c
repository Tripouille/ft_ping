#include "initialize.h"

static bool
packet_has_been_received(void) {
	for (t_list_element * element = g_ping.stats.head;
	element != g_ping.stats.tail; element = element->next)
		if (element->data.received) return (true);
	return (g_ping.stats.tail->data.received);
}

void
display_statistics_exit(int signal) {
	(void)signal;
	struct timeval	now;

	gettimeofday(&now, NULL);
    printf("\n--- %s ping statistics ---\n", g_ping.host);
	printf("%li packets transmitted, %li received, ", g_ping.msg_count, g_ping.msg_received_count);
	if (g_ping.duplicate) printf("+%li duplicates, ", g_ping.duplicate);
	if (g_ping.error) printf("+%li errors, ", g_ping.error);
	printf("%g%% packet loss, time %.fms\n", 100.0 - (g_ping.msg_received_count / (double)g_ping.msg_count * 100),
		get_elapsed_us(&g_ping.start, &now) / 1E3);
	if (packet_has_been_received()) {
		printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
			list_get_smallest(&g_ping.stats), list_get_average(&g_ping.stats),
			list_get_biggest(&g_ping.stats), list_get_mdev(&g_ping.stats));
	}
	free(g_ping.sent_packet_tracker);
	free(g_ping.recv_buffer);
	list_destroy(&g_ping.stats);
	exit(EXIT_SUCCESS);
}

static void
print_argument_garbage(char const * value) {
	printf("ft_ping: option argument contains garbage: %s\n", value);
}

static void
initialize_options(void) {
	t_option *		option;

	if ((option = get_option(g_ping.options, 's'))->active)
		g_ping.packet_msg_size = parse_int(option->value, 0, INT_MAX);
	if ((option = get_option(g_ping.options, 't'))->active) {
		g_ping.ttl = parse_int(option->value, 0, 255);
		if (!g_ping.ttl) print_error_exit("ft_ping: cannot set unicast time-to-live: Invalid argument");
	}
	if ((option = get_option(g_ping.options, 'i'))->active) {
		char * end;
		g_ping.interval_second = strtod(option->value, &end);
		if (option->value + slen(option->value) != end) print_argument_garbage(end);
		if (g_ping.interval_second < 0.2) print_error_exit("ft_ping: cannot flood, minimal interval allowed is 200ms");
	}
	if ((option = get_option(g_ping.options, 'c'))->active) {
		g_ping.count = parse_int(option->value, 1, INT_MAX);
	}
}


void
initialize_socket(void) {
    struct timeval	timeout = {0, 1E4};

    setsockopt(g_ping.socket_fd, SOL_IP, IP_TTL, &g_ping.ttl, sizeof(g_ping.ttl));
    setsockopt(g_ping.socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}


void
initialize_packet(void * packet, size_t packet_size) {
	struct icmphdr *	header = packet;

    mset(packet, packet_size, 0);
    header->type = ICMP_ECHO;
    header->un.echo.id = g_ping.pid;
}

void
initialize_msg(struct msghdr * msg, struct iovec * iov) {
	mset(msg, sizeof(*msg), 0);
	msg->msg_iov = iov;
	msg->msg_iovlen = 1;
}

void
initialize_config(char ** av) {
	mset(&g_ping, sizeof(g_ping), 0);
	g_ping.packet_msg_size = 56;
	g_ping.ttl = 64;
	g_ping.interval_second = 1;
	g_ping.count = -1;
	load_available_options(g_ping.options);
	parse_arguments(av + 1);
	if (get_option(g_ping.options, 'h')->active) usage();
	initialize_options();
	if (g_ping.host == NULL) usage();
	g_ping.socket_fd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (g_ping.socket_fd == -1) print_error_exit("Socket file descriptor not received");
    g_ping.pid = getpid();
	list_initialize(&g_ping.stats);
	signal(SIGINT, display_statistics_exit);
}