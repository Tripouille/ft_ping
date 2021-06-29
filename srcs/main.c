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
	if (get_option(g_ping.options, 'v')->active) {
		if (!get_option(g_ping.options, 'n')->active)
			printf("From %s (%s) icmp_seq=%li %s\n", reverse_dns_lookup(sender_ip) ? g_ping.reverse_dns : sender_ip, sender_ip,
				g_ping.msg_count, get_type_information(icmp_header->type));
		else
			printf("From %s icmp_seq=%li %s\n", sender_ip, g_ping.msg_count, get_type_information(icmp_header->type));
	}
}

static void
display_reply(double time, struct iphdr * ip_header, struct icmphdr * icmp_header,
char const * sender_ip, size_t recv_packet_size, bool already_received) {
	if (!get_option(g_ping.options, 'n')->active && reverse_dns_lookup(sender_ip))
		printf("%li bytes from %s (%s): msg_seq=%i ttl=%i time=%.1f ms",
			recv_packet_size - sizeof(struct iphdr), g_ping.reverse_dns, sender_ip,
			icmp_header->un.echo.sequence, ip_header->ttl, time);
	else
		printf("%li bytes from %s: msg_seq=%i ttl=%i time=%.1f ms",
			recv_packet_size - sizeof(struct iphdr), sender_ip,
			icmp_header->un.echo.sequence, ip_header->ttl, time);
	if (already_received) {
		++g_ping.duplicate;
		printf(" (DUP!)\n");
	} else {
		++g_ping.msg_received_count;
		printf("\n");
	}
}

static void
wait_ping_reply(size_t packet_size) {
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
		&& icmp_header->un.echo.id == g_ping.pid
		&& checksum(icmp_header, recv_packet_size - sizeof(struct iphdr)) == recv_checksum) {
			t_packet_tracker * tracker = list_get_tracker(&g_ping.stats, icmp_header->un.echo.sequence);
			struct timeval now;

			if (tracker == NULL) return ;
			gettimeofday(&now, NULL);
			tracker->travel_time = get_elapsed_us(&tracker->sent_timeval, &now) / 1E3;
			if (!get_option(g_ping.options, 'q')->active)
				display_reply(tracker->travel_time, ip_header, icmp_header, sender_ip, recv_packet_size, tracker->received);
			tracker->received = true;
		} else if (icmp_header->code == ICMP_ECHOREPLY && ip_header->daddr != g_ping.addr_con.sin_addr.s_addr) {
			if (!get_option(g_ping.options, 'q')->active)
				display_type_information(icmp_header, sender_ip);
			++g_ping.error;
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
	struct timeval		now;
	t_packet_tracker	tracker;

	if ((g_ping.sent_packet_tracker = malloc(packet_size)) == NULL) print_error_exit("ft_ping: Out of memory");
	if ((g_ping.recv_buffer = malloc(sizeof(struct iphdr) + packet_size)) == NULL) print_error_exit("ft_ping: Out of memory");
	printf("PING %s (%s) %li(%li) bytes of data.\n", g_ping.host, g_ping.ip, g_ping.packet_msg_size, sizeof(struct iphdr) + packet_size);
	initialize_packet(g_ping.sent_packet_tracker, packet_size);
	gettimeofday(&g_ping.start, NULL);
	while(g_ping.msg_count < g_ping.count || !get_option(g_ping.options, 'c')->active) {
		actualize_packet(g_ping.sent_packet_tracker, packet_size);
		gettimeofday(&tracker.sent_timeval, NULL);
		tracker.sequence = g_ping.msg_count;
		tracker.received = false;
		if (list_push(&g_ping.stats, tracker) == NULL) print_error_exit("ft_ping: Out of memory");
		gettimeofday(&now, NULL);
		if (sendto(g_ping.socket_fd, g_ping.sent_packet_tracker, packet_size, 0,
		(struct sockaddr*)&g_ping.addr_con, sizeof(g_ping.addr_con)) != -1) {
			do {
				wait_ping_reply(packet_size);
				gettimeofday(&now, NULL);
			} while (get_elapsed_us(&tracker.sent_timeval, &now) / 1E6 < g_ping.interval_second);
		}
	}
	display_statistics_exit(0);
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