#ifndef UTILS_H
# define UTILS_H
# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>
# include <stddef.h>

void
print_error_exit(char const * msg);

bool
is_digit(char c);

bool
is_full_digit(char const * s);

#endif