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
	g_ping.active = false;
    fprintf(stderr, "\n--- %s ping statistics ---", g_ping.host);
	exit(EXIT_SUCCESS);
}

void
initialize_config(char ** av) {
	g_ping.host = NULL;
	initialize_options(g_ping.options);
	parse_arguments(av + 1);
	if (g_ping.host == NULL || get_option(g_ping.options, 'h')->active) usage();
	g_ping.socket_fd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (g_ping.socket_fd == -1) print_error_exit("Socket file descriptor not received");
    g_ping.pid = getpid();
	g_ping.active = true;
    g_ping.msg_count = 0;
    g_ping.msg_received_count = 0;
	signal(SIGINT, signal_handler);
	print_options();
}

void
initialize_socket(void) {
    struct timeval timeout = {1, 0};

    g_ping.ttl = 64;
    setsockopt(g_ping.socket_fd, SOL_IP, IP_TTL, &g_ping.ttl, sizeof(g_ping.ttl));
    setsockopt(g_ping.socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}


void
initialize_packet(t_packet * packet) {
    mset(packet, sizeof(*packet), 0);
    packet->hdr.type = ICMP_ECHO;
    packet->hdr.un.echo.id = g_ping.pid;
    packet->hdr.un.echo.sequence = g_ping.msg_count;
    packet->hdr.checksum = checksum(packet, sizeof(*packet));
}

void
initialize_msg(struct msghdr * msg, struct iovec * iov) {
	mset(msg, sizeof(*msg), 0);
	msg->msg_iov = iov;
	msg->msg_iovlen = 1;
}