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

static void
send_ping(void) {
    t_packet packet;
    struct timeval start, end;

    printf("PING %s (%s) %i(%li) bytes of data.\n", g_ping.host, g_ping.ip,
            PACKET_MESSAGE, IPV4_HEADER + PACKET_SIZE);
    while(g_ping.active) {
        initialize_packet(&packet);
        gettimeofday(&start, NULL);
        if (sendto(g_ping.socket_fd, &packet, sizeof(packet), 0,
        (struct sockaddr*)&g_ping.addr_con, sizeof(g_ping.addr_con)) != -1) {
            struct msghdr msg;
            struct iovec iov = {&packet, PACKET_SIZE};
            initialize_msg(&msg, &iov);
            if (recvmsg(g_ping.socket_fd, &msg, 0) != -1) {
                gettimeofday(&end, NULL);
                if ((packet.hdr.type == 69 && packet.hdr.code == 0)) {
                    printf("%ld bytes from %s (%s) msg_seq=%ld ttl=%d time=%.3f ms.\n", PACKET_SIZE,
                            g_ping.host, g_ping.ip, g_ping.msg_count, g_ping.ttl, get_elapsed_us(&start, &end) / 1E3);
                    g_ping.msg_received_count++;
                }
            }
        }
        sleep(1);
    }
}

int
main(int ac, char ** av) {
	(void)ac;
	initialize_config(av);
	dns_lookup();
    initialize_socket();
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