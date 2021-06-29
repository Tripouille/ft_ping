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

static char const * 
get_type_information(int type) {
	if (type == ICMP_DEST_UNREACH) return ("Destination Unreachable");
	if (type == ICMP_SOURCE_QUENCH)	return ("Source Quench");
	if (type == ICMP_REDIRECT) return ("Redirect (change route)");
	if (type == ICMP_ECHO) return ("Echo Request"); 
	if (type == ICMP_TIME_EXCEEDED)	return ("Time to live exceeded");
	if (type == ICMP_PARAMETERPROB)	return ("Parameter Problem");
	if (type == ICMP_TIMESTAMP)	return ("Timestamp Request");
	if (type == ICMP_TIMESTAMPREPLY) return ("Timestamp Reply");
	if (type == ICMP_INFO_REQUEST) return ("Information Request");
	if (type == ICMP_INFO_REPLY) return ("Information Reply");
	if (type == ICMP_ADDRESS) return ("Address Mask Request");
	if (type == ICMP_ADDRESSREPLY) return ("Address Mask Reply");
	return ("Unknow type");
}

static void
display_type_information(struct icmphdr * icmp_header, char const * sender_ip) {
	if (get_option(g_ping.options, 'v')->active)
		printf("From %s (%s) icmp_seq=%li %s\n",
			reverse_dns_lookup(sender_ip) ? g_ping.reverse_dns : sender_ip, sender_ip,
			g_ping.msg_count, get_type_information(icmp_header->type));
	++g_ping.error;
}

static void
display_reply(double time, struct iphdr * ip_header, struct icmphdr * icmp_header,
char const * sender_ip, size_t recv_packet_size) {
	if (list_push(&g_ping.stats, time) == NULL) print_error_exit("ft_ping: Out of memory");
	if (reverse_dns_lookup(sender_ip))
		printf("%li bytes from %s (%s): msg_seq=%i ttl=%i time=%.1f ms",
			recv_packet_size - sizeof(struct iphdr), g_ping.reverse_dns, sender_ip,
			icmp_header->un.echo.sequence, ip_header->ttl, time);
	else
		printf("%li bytes from %s: msg_seq=%i ttl=%i time=%.1f ms",
			recv_packet_size - sizeof(struct iphdr), sender_ip,
			icmp_header->un.echo.sequence, ip_header->ttl, time);
	if (icmp_header->un.echo.sequence > g_ping.last_sequence_received) {
		g_ping.last_sequence_received = icmp_header->un.echo.sequence;
		++g_ping.msg_received_count;
		printf("\n");
	} else if (icmp_header->un.echo.sequence == g_ping.last_sequence_received) {
		++g_ping.duplicate;
		printf(" (DUP!)\n");
	}
}

static void
wait_ping_reply(struct timeval const start_cache[], size_t packet_size) {
	struct iphdr *			ip_header;
	struct icmphdr *		icmp_header;
	ssize_t					recv_packet_size;
	char					sender_ip[NI_MAXHOST];

	struct msghdr msg;
	struct iovec iov = {g_ping.recv_buffer, sizeof(struct iphdr) + packet_size};
	initialize_msg(&msg, &iov);
	if ((recv_packet_size = recvmsg(g_ping.socket_fd, &msg, 0)) != -1) {
		ip_header = g_ping.recv_buffer;
		icmp_header = g_ping.recv_buffer + sizeof(struct iphdr);
		uint16_t recv_checksum = icmp_header->checksum;

		icmp_header->checksum = 0;
		inet_ntop(AF_INET, &ip_header->saddr, sender_ip, sizeof(sender_ip));
		if (icmp_header->type == ICMP_ECHOREPLY && icmp_header->code == ICMP_ECHOREPLY
		&& icmp_header->un.echo.id == g_ping.pid && icmp_header->un.echo.sequence > 0
		&& checksum(icmp_header, recv_packet_size - sizeof(struct iphdr)) == recv_checksum) {
			struct timeval now;

			gettimeofday(&now, NULL);
			display_reply(get_elapsed_us(start_cache + (icmp_header->un.echo.sequence - 1) % START_CACHE, &now) / 1E3,
				ip_header, icmp_header, sender_ip, recv_packet_size);
		} else if (icmp_header->code == ICMP_ECHOREPLY)
			display_type_information(icmp_header, sender_ip);
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
	struct timeval		start_cache[START_CACHE], now;

	if ((g_ping.sent_packet = malloc(packet_size)) == NULL) print_error_exit("ft_ping: Out of memory");
	if ((g_ping.recv_buffer = malloc(sizeof(struct iphdr) + packet_size)) == NULL) print_error_exit("ft_ping: Out of memory");
	printf("PING %s (%s) %li(%li) bytes of data.\n", g_ping.host, g_ping.ip, g_ping.packet_msg_size, sizeof(struct iphdr) + packet_size);
	initialize_packet(g_ping.sent_packet, packet_size);
	gettimeofday(&g_ping.start, NULL);
	while(true) {
		struct timeval * actual_start = start_cache + (g_ping.msg_count - 1) % START_CACHE;
		gettimeofday(actual_start, NULL);
		gettimeofday(&now, NULL);
		if (sendto(g_ping.socket_fd, g_ping.sent_packet, packet_size, 0,
		(struct sockaddr*)&g_ping.addr_con, sizeof(g_ping.addr_con)) != -1) {
			while (get_elapsed_us(actual_start, &now) / 1E6 < g_ping.interval_second) {
				wait_ping_reply(start_cache, packet_size);
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