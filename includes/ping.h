#ifndef PING_H
# define PING_H

# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
# include <sys/socket.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <stdbool.h>
# include <unistd.h>

# include "utils.h"
# include "options.h"

typedef struct  s_ping_infos
{
	char *      host;
	int			socket_fd;
	bool        active;
	t_option	options[OPTION_NUMBER];
}               t_ping_infos;

#endif