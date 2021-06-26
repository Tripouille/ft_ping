#ifndef INITIALIZE_H
# define INITIALIZE_H
# include <signal.h>
# include "ping.h"
# include "utils.h"

void
initialize_config(char ** av);

void
initialize_ping(void);

void
initialize_packet(t_packet * packet);

#endif