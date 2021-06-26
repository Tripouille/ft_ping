#ifndef INITIALIZE_H
# define INITIALIZE_H
# include <signal.h>
# include "ping.h"
# include "utils.h"

void
initialize_config(char ** av);

void
initialize_socket(void);

void
initialize_packet(t_packet * packet);

void
initialize_msg(struct msghdr * msg, struct iovec * iov);

#endif