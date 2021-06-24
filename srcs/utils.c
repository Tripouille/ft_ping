#include "utils.h"

void
print_error_exit(char const * msg) {
	dprintf(2, "%s\n", msg);
	exit(EXIT_FAILURE);
}

bool
is_digit(char c) {
	return (c >= '0' && c <= '9');
}

bool
is_full_digit(char const * s) {
	for (int i = 0; s[i]; ++i)
		if (!is_digit(s[i]))
			return (false);
	return (true);
}