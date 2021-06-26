#include "ping.h"

t_ping_infos	g_ping_infos;

static void
signal_handler(int signal) {
	(void)signal;
	g_ping_infos.active = false;
	printf("\rDone!\n");
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

static void
dns_lookup(void) {
	struct addrinfo * info;
	struct sockaddr_in * address;

    if (getaddrinfo(g_ping_infos.host, NULL, NULL, &info)) {
		fprintf(stderr, "ping: %s: Name or service not known\n", g_ping_infos.host);
		exit(EXIT_FAILURE);
	}
	address = (struct sockaddr_in*)info->ai_addr;
	inet_ntop(AF_INET, &address->sin_addr, g_ping_infos.ip, sizeof(g_ping_infos.ip));

    g_ping_infos.addr_con.sin_family = info->ai_family;
    g_ping_infos.addr_con.sin_port = htons(0);
    g_ping_infos.addr_con.sin_addr.s_addr = *(in_addr_t*)&address->sin_addr;
}

static void
initialize(char ** av) {
	g_ping_infos.host = NULL;
	g_ping_infos.active = true;
	initialize_options(g_ping_infos.options);
	parse_arguments(av + 1);
	if (g_ping_infos.host == NULL
	|| get_option(g_ping_infos.options, 'h')->active)
		usage();
	g_ping_infos.socket_fd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (g_ping_infos.socket_fd == -1)
	 	print_error_exit("Socket file descriptor not received");
	signal(SIGINT, signal_handler);
	print_options();
}

unsigned short checksum(void *b, int len)
{    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;
  
    for ( sum = 0; len > 1; len -= 2 )
        sum += *buf++;
    if ( len == 1 )
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void
send_ping(void) {
    int ttl = 64, msg_count = 0, flag = 1, msg_received_count = 0;
    struct paquet pckt;
    struct sockaddr_in r_addr;
    struct timespec time_start, time_end, tfs, tfe;
    long double rtt_msec=0, total_msec=0;
	socklen_t addr_len;
    struct timeval timeout = {10, 0};

    clock_gettime(CLOCK_MONOTONIC, &tfs);
    setsockopt(g_ping_infos.socket_fd, SOL_IP, IP_TTL, &ttl, sizeof(ttl));
    setsockopt(g_ping_infos.socket_fd, SOL_SOCKET, SO_RCVTIMEO,
				(char const*)&timeout, sizeof(timeout));

    while(g_ping_infos.active)
    {
        flag=1;
        bzero(&pckt, sizeof(pckt));
        pckt.hdr.type = ICMP_ECHO;
        pckt.hdr.un.echo.id = getpid();
          
        for (unsigned long i = 0; i < sizeof(pckt.message)-1; i++ )
            pckt.message[i] = i+'0';
        pckt.hdr.un.echo.sequence = msg_count++;
        pckt.hdr.checksum = checksum(&pckt, sizeof(pckt));
  
        sleep(1);
  
        clock_gettime(CLOCK_MONOTONIC, &time_start);
        if (sendto(g_ping_infos.socket_fd, &pckt, sizeof(pckt), 0, (struct sockaddr*)&g_ping_infos.addr_con, sizeof(g_ping_infos.addr_con)) <= 0)
        {
            printf("\nPacket Sending Failed!\n");
            flag=0;
        }
  
        //receive packet
        addr_len=sizeof(r_addr);
  
        if (recvfrom(g_ping_infos.socket_fd, &pckt, sizeof(pckt), 0, (struct sockaddr*)&r_addr, &addr_len) <= 0 && msg_count > 1) 
        {
            printf("\nPacket receive failed!\n");
        }
        else
        {
            clock_gettime(CLOCK_MONOTONIC, &time_end);
              
            double timeElapsed = ((double)(time_end.tv_nsec - time_start.tv_nsec)) / 1000000.0;
            rtt_msec = (time_end.tv_sec - time_start.tv_sec) * 1000.0 + timeElapsed;
              
            // if packet was not sent, don't receive
            if(flag)
            {
                if(!(pckt.hdr.type ==69 && pckt.hdr.code==0)) 
                {
                    printf("Error..Packet received with ICMP type %d code %d\n", pckt.hdr.type, pckt.hdr.code);
                }
                else
                {
                    printf("%d bytes from %s (%s) msg_seq=%d ttl=%d rtt = %Lf ms.\n", 42, g_ping_infos.host, g_ping_infos.ip, msg_count, ttl, rtt_msec);
                    msg_received_count++;
                }
            }
        }    
    }
    clock_gettime(CLOCK_MONOTONIC, &tfe);
    double timeElapsed = ((double)(tfe.tv_nsec - tfs.tv_nsec)) / 1000000.0;
    total_msec = (tfe.tv_sec-tfs.tv_sec) * 1000.0 + timeElapsed;        
    printf("\n===%s ping statistics===\n", g_ping_infos.ip);
	(void)total_msec;
    //printf("\n%d packets sent, %d packets received, %f percentpacket loss. Total time: %Lf ms.\n\n", msg_count, msg_received_count, ((msg_count - msg_received_count)/msg_count) * 100.0,total_msec); 
}


int
main(int ac, char ** av) {
	(void)ac;
	initialize(av);
	dns_lookup();
    printf("Trying to connect to '%s' IP: %s\n", g_ping_infos.host, g_ping_infos.ip);
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