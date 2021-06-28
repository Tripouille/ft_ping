#ifndef OPTIONS_H
# define OPTIONS_H
# include <stdbool.h>
# include <stddef.h>
# include <limits.h>

# include "ping.h"

void
load_available_options(t_option * options);

t_option *
get_option(t_option options[], char id);

#endif