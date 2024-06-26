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

# define OPTION_NUMBER 8

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
	char					ip[NI_MAXHOST];
	int						pid;
	ssize_t					msg_count;
	ssize_t					msg_received_count;
	ssize_t					packet_msg_size;
	void *					sent_packet_tracker;
	void *					recv_buffer;
	t_list					stats;
	struct timeval			start;
	char					reverse_dns[NI_MAXHOST];
	ssize_t					duplicate;
	ssize_t					error;
	int						ttl;
	int						count;
	double					interval_second;
}               t_ping_infos;

extern t_ping_infos g_ping;

#endif