#ifndef INITIALIZE_H
# define INITIALIZE_H
# include <signal.h>
# include <stdlib.h>

# include "ping.h"
# include "utils.h"
# include "options.h"


void
initialize_config(char ** av);

void
initialize_socket(void);

void
initialize_packet(void * packet, size_t packet_size);

void
initialize_msg(struct msghdr * msg, struct iovec * iov);

#endif