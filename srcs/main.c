#include "ping.h"
#include "initialize.h"
#include "utils.h"
#include "parsing.h"

t_ping_infos	g_ping;

static void
dns_lookup(void) {
	struct addrinfo * info;
	struct sockaddr_in * address;

	if (getaddrinfo(g_ping.host, NULL, NULL, &info)) {
		fprintf(stderr, "ping: %s: Name or service not known\n", g_ping.host);
		exit(EXIT_FAILURE);
	}
	address = (struct sockaddr_in*)info->ai_addr;
	inet_ntop(AF_INET, &address->sin_addr, g_ping.ip, sizeof(g_ping.ip));
	g_ping.addr_con.sin_family = info->ai_family;
	g_ping.addr_con.sin_port = htons(0);
	g_ping.addr_con.sin_addr.s_addr = *(in_addr_t*)&address->sin_addr;
	freeaddrinfo(info);
}

static bool
wait_ping_reply(struct timeval const * start, struct timeval * end) {
	char			recv_buffer[TOTAL_PACKET_SIZE];
	t_packet		recv_packet;

	struct msghdr msg;
	struct iovec iov = {recv_buffer, TOTAL_PACKET_SIZE};
	initialize_msg(&msg, &iov);
	if (recvmsg(g_ping.socket_fd, &msg, MSG_WAITALL) != -1) {
		recv_packet = *(t_packet*)(recv_buffer + IPV4_HEADER);
		if (recv_packet.hdr.type == ICMP_ECHOREPLY && recv_packet.hdr.code == ICMP_ECHOREPLY
		&& recv_packet.hdr.un.echo.id == g_ping.pid) {
			gettimeofday(end, NULL);
			printf("%ld bytes from %s (%s) msg_seq=%ld ttl=%d time=%.3f ms.\n", PACKET_SIZE,
					g_ping.host, g_ping.ip, g_ping.msg_count, g_ping.ttl, get_elapsed_us(start, end) / 1E3);
			g_ping.msg_received_count++;
			return (false);
		}
	}
	return (true);
}

static void
send_ping_request(void) {
	t_packet		send_packet;
	struct timeval	start, end, now;

	printf("PING %s (%s) %i(%li) bytes of data.\n", g_ping.host, g_ping.ip,
			PACKET_MESSAGE, IPV4_HEADER + PACKET_SIZE);
	initialize_packet(&send_packet);
	while(g_ping.active) {
		gettimeofday(&start, NULL);
		now = start;
		if (sendto(g_ping.socket_fd, &send_packet, sizeof(send_packet), 0,
		(struct sockaddr*)&g_ping.addr_con, sizeof(g_ping.addr_con)) != -1)
			while (wait_ping_reply(&start, &end)
			&& get_elapsed_us(&start, &now) < PING_REQUEST_TIMEOUT_US)
				gettimeofday(&now, NULL);
		while (get_elapsed_us(&start, &now) < PING_REQUEST_DELAY_US)
			gettimeofday(&now, NULL);
		g_ping.msg_count++;
	}
}

int
main(int ac, char ** av) {
	(void)ac;
	if (getuid())
		print_error_exit("ft_ping: must be run as root.");
	initialize_config(av);
	dns_lookup();
	initialize_socket();
	send_ping_request();
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