#ifndef UTILS_H
# define UTILS_H
# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>
# include <stddef.h>

# include "ping.h"

void
print_error_exit(char const * msg);

bool
is_digit(char c);

bool
is_full_digit(char const * s);

void
mset(void * m, size_t size, char value);

unsigned short
checksum(void *b, int len);

void
parse_arguments(char ** args);

void
usage(void);

#endif