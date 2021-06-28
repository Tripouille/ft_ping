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
# include <sys/time.h>
# include <time.h>
# include <string.h>

# include "list.h"

# define IPV4_HEADER 20
# define PING_REQUEST_TIMEOUT_US (1E6)
# define PING_REQUEST_DELAY_US (1E6)
# define TTL 64
# define OPTION_NUMBER 3


typedef struct	s_option
{
	char		id;
    bool        active;
	bool		require_value;
	char *		value;
}				t_option;

typedef struct  s_ping_infos
{
	char *      			host;
	int						socket_fd;
	struct sockaddr_in		addr_con;
	t_option				options[OPTION_NUMBER];
	char					ip[NI_MAXHOST + 1];
	int						pid;
	size_t					msg_count;
	size_t					msg_received_count;
	size_t					packet_msg_size;
	void *					sent_packet;
	char *					recv_buffer;
	t_list					stats;
	struct timeval			start;
}               t_ping_infos;

extern t_ping_infos g_ping;

#endif