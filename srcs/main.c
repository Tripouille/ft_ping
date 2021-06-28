#include "ping.h"
#include "initialize.h"
#include "utils.h"
#include "parsing.h"

t_ping_infos	g_ping;

static bool
reverse_dns_lookup(char const * ip_addr) {
    struct sockaddr_in addr_in;    
  
    addr_in.sin_family = AF_INET;
	inet_pton(AF_INET, ip_addr, &addr_in.sin_addr.s_addr);
    return (!getnameinfo((struct sockaddr*)&addr_in, sizeof(struct sockaddr_in),
	g_ping.reverse_dns, sizeof(g_ping.reverse_dns), NULL, 0, NI_NAMEREQD));
}

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

static void
wait_ping_reply(struct timeval const * start, struct timeval * end, size_t packet_size) {
	struct iphdr *			ip_header;
	struct icmphdr *		icmp_header;
	ssize_t					recv_packet_size;
	char					sender_ip[NI_MAXHOST];

	struct msghdr msg;
	struct iovec iov = {g_ping.recv_buffer, IPV4_HEADER + packet_size};
	initialize_msg(&msg, &iov);
	if ((recv_packet_size = recvmsg(g_ping.socket_fd, &msg, 0)) != -1) {
		ip_header = g_ping.recv_buffer;
		icmp_header = g_ping.recv_buffer + IPV4_HEADER;
		uint16_t recv_checksum = icmp_header->checksum;

		icmp_header->checksum = 0;
		inet_ntop(AF_INET, &ip_header->saddr, sender_ip, sizeof(sender_ip));
		if (icmp_header->type == ICMP_ECHOREPLY && icmp_header->code == ICMP_ECHOREPLY
		&& icmp_header->un.echo.id == g_ping.pid
		&& checksum(icmp_header, recv_packet_size - IPV4_HEADER) == recv_checksum) {
			gettimeofday(end, NULL);
			double time = get_elapsed_us(start, end) / 1E3;
			if (list_push(&g_ping.stats, time) == NULL) print_error_exit("ft_ping: Out of memory");
			if (reverse_dns_lookup(sender_ip))
				printf("%li bytes from %s (%s): msg_seq=%i ttl=%i time=%.1f ms",
						recv_packet_size - IPV4_HEADER, g_ping.reverse_dns, sender_ip,
						icmp_header->un.echo.sequence, ip_header->ttl, time);
			else
				printf("%li bytes from %s: msg_seq=%i ttl=%i time=%.1f ms",
							recv_packet_size - IPV4_HEADER, sender_ip,
							icmp_header->un.echo.sequence, ip_header->ttl, time);
			if (icmp_header->un.echo.sequence > g_ping.last_sequence_received) {
				g_ping.last_sequence_received = icmp_header->un.echo.sequence;
				++g_ping.msg_received_count;
				printf("\n");
			} else {
				++g_ping.duplicate;
				printf(" (DUP!)\n");
			}
		}
	}
}

static void
actualize_packet(void * packet, size_t packet_size) {
	struct icmphdr *	header = packet;

	header->checksum = 0;
    header->un.echo.sequence = ++g_ping.msg_count;
    header->checksum = checksum(packet, packet_size);
}

static void
send_ping_request(void) {
	size_t				packet_size = sizeof(struct icmphdr) + g_ping.packet_msg_size;
	struct timeval		start, end, now;

	if ((g_ping.sent_packet = malloc(packet_size)) == NULL) print_error_exit("ft_ping: Out of memory");
	if ((g_ping.recv_buffer = malloc(IPV4_HEADER + packet_size)) == NULL) print_error_exit("ft_ping: Out of memory");
	printf("PING %s (%s) %li(%li) bytes of data.\n", g_ping.host, g_ping.ip,
		g_ping.packet_msg_size, IPV4_HEADER + packet_size);
	initialize_packet(g_ping.sent_packet, packet_size);
	gettimeofday(&g_ping.start, NULL);
	while(true) {
		gettimeofday(&start, NULL);
		now = start;
		if (sendto(g_ping.socket_fd, g_ping.sent_packet, packet_size, 0,
		(struct sockaddr*)&g_ping.addr_con, sizeof(g_ping.addr_con)) != -1) {
			while (get_elapsed_us(&start, &now) < PING_REQUEST_DELAY_US) {
				wait_ping_reply(&start, &end, packet_size);
				gettimeofday(&now, NULL);
			}
		}
		actualize_packet(g_ping.sent_packet, packet_size);
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