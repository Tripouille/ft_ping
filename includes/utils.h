#ifndef UTILS_H
# define UTILS_H
# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>
# include <stddef.h>
# include <limits.h>

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
checksum(void * data, size_t len);

void
parse_arguments(char ** args);

void
usage(void);

size_t
get_elapsed_us(struct timeval const * start, struct timeval const * end);

int
parse_int(char const *s, int min, int max);

#endif