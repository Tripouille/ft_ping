#include "ping.h"

t_ping_infos	g_ping;

static void
signal_handler(int signal) {
	(void)signal;
	g_ping.active = false;
    fprintf(stderr, "\n--- %s ping statistics ---", g_ping.host);
	exit(EXIT_SUCCESS);
}

static void
usage(void) {
	char const *	message = "usage: ping [-vh] host";

	fprintf(stderr, "%s\n", message);
	exit(EXIT_FAILURE);
}

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
print_options(void) {
	for (int i = 0; i < OPTION_NUMBER; ++i) {
		printf("option %c: active = %i, value = %s\n", g_ping.options[i].id, g_ping.options[i].active, g_ping.options[i].value);
	}
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

static unsigned short
checksum(void *b, int len) {   
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;
  
    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1) sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

static void
initialize_ping(void) {
    struct timeval timeout = {10, 10};

    g_ping.ttl = 64;
    setsockopt(g_ping.socket_fd, SOL_IP, IP_TTL, &g_ping.ttl, sizeof(g_ping.ttl));
    setsockopt(g_ping.socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

static void
initialize_packet(t_packet * packet) {
    mset(packet, sizeof(packet), 0);
    packet->hdr.type = ICMP_ECHO;
    packet->hdr.un.echo.id = g_ping.pid;
    packet->hdr.un.echo.sequence = g_ping.msg_count++;
    packet->hdr.checksum = checksum(&packet, sizeof(packet));
}

static void
send_ping(void) {
    struct timespec time_start, time_end;
    t_packet packet;
    //struct sockaddr_in r_addr;
    //socklen_t addr_len=sizeof(r_addr);

    while(g_ping.active) {
        initialize_packet(&packet);
        clock_gettime(CLOCK_MONOTONIC, &time_start);
        if (sendto(g_ping.socket_fd, &packet, sizeof(packet), 0,
        (struct sockaddr*)&g_ping.addr_con, sizeof(g_ping.addr_con)) != -1) {
            struct msghdr msg;
            mset(&msg, sizeof(msg), 0);
            struct iovec iov = {&packet, PACKET_SIZE};
            msg.msg_iov = &iov;
            msg.msg_iovlen = 1;

            if (recvmsg(g_ping.socket_fd, &msg, 0) != -1) {
                clock_gettime(CLOCK_MONOTONIC, &time_end);
                if (!(packet.hdr.type == 69 && packet.hdr.code == 0)) 
                    printf("Error..Packet received with ICMP type %d code %d\n", packet.hdr.type, packet.hdr.code);
                else {
                    printf("%ld bytes from %s (%s) msg_seq=%ld ttl=%d rtt = %f ms.\n", PACKET_SIZE,
                            g_ping.host, g_ping.ip, g_ping.msg_count, g_ping.ttl, 0.1);
                    g_ping.msg_received_count++;
                }
            }
            else
                printf("oups\n");
        }
        sleep(1);
    }
}

int
main(int ac, char ** av) {
	(void)ac;
	initialize_config(av);
	dns_lookup();
    printf("PING %s (%s) %i(%li) bytes of data.\n", g_ping.host, g_ping.ip,
            PACKET_MESSAGE, IPV4_HEADER + PACKET_SIZE);
    initialize_ping();
	send_ping();
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