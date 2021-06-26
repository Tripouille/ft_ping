#ifndef PING_H
# define PING_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdbool.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <arpa/inet.h>
# include <time.h>
# include <string.h>

# include "options.h"

# define IPV4_HEADER 20
# define PACKET_MESSAGE 56
# define PACKET_SIZE (PACKET_MESSAGE + sizeof(struct icmphdr))

typedef struct	s_packet
{
    struct icmphdr	hdr;
    char			message[PACKET_SIZE];
}				t_packet;

typedef struct  s_ping_infos
{
	char *      			host;
	int						socket_fd;
	struct sockaddr_in		addr_con;
	t_option				options[OPTION_NUMBER];
	char					ip[NI_MAXHOST + 1];
	bool        			active;
	int						ttl;
	int						pid;
	size_t					msg_count;
	size_t					msg_received_count;
}               t_ping_infos;

extern t_ping_infos g_ping;

#endif