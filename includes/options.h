#ifndef OPTIONS_H
# define OPTIONS_H
# define OPTION_NUMBER 3
# include <stdbool.h>
# include <stddef.h>


typedef struct	s_option
{
	char		id;
    bool        active;
	bool		require_value;
	char *		value;
}				t_option;

void
initialize_options(t_option * options);

t_option *
get_option(t_option options[], char id);

#endif