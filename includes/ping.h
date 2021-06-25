#ifndef PING_H
# define PING_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdbool.h>
# include <fcntl.h>
# include <signal.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <arpa/inet.h>

# include "utils.h"
# include "options.h"

typedef struct  s_ping_infos
{
	char *      			host;
	int						socket_fd;
	struct sockaddr_in		addr_con;
	t_option				options[OPTION_NUMBER];
	char					ip[NI_MAXHOST + 1];
	bool        			active;
}               t_ping_infos;

#endif