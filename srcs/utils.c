#include "utils.h"

void
print_error_exit(char const * msg) {
	free(g_ping.sent_packet);
	free(g_ping.recv_buffer);
	list_destroy(&g_ping.stats);
	fprintf(stderr, "%s\n", msg);
	exit(EXIT_FAILURE);
}

bool
is_digit(char c) {
	return (c >= '0' && c <= '9');
}

bool
is_full_digit(char const * s) {
	for (int i = 0; s[i]; ++i)
		if (!is_digit(s[i])) return (false);
	return (true);
}

void
mset(void * m, size_t size, char value) {
	while (size--) ((char*)m)[size] = value;
}

unsigned short
checksum(void * data, size_t len) {   
	unsigned short const * buffer = data;
	unsigned int sum;
  
	for (sum = 0; len > 1; len -= 2) sum += *buffer++;
	if (len == 1) sum += *(unsigned char*)buffer;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	return (~sum);
}

double
get_elapsed_us(struct timeval const * start, struct timeval const * end) {
	return ((end->tv_sec - start->tv_sec) * 1E6 + (end->tv_usec - start->tv_usec));
}

int
parse_int(char const * s, int min, int max)
{
	long		value = 0;
	size_t	i = (s[0] == '-' || s[0] == '+') ? 1 : 0;;
	bool    is_negative = s[0] == '-';

	if (!s[i]) {
		fprintf(stderr, "ft_ping: invalid argument: %s\n", s);
		exit(EXIT_FAILURE);
	}
	while (s[i] && s[i] >= '0' && s[i] <= '9'
	&& ((!is_negative && value <= max) || (is_negative && -value >= min)))
		value = value * 10 + s[i++] - '0';
	if (s[i]) {
		if ((!is_negative && value > max) || (is_negative && -value < min))
			fprintf(stderr, "ft_ping: invalid argument: %s: out of range: %i <= value <= %i\n", s, min, max);
		else
			fprintf(stderr, "ft_ping: invalid argument: %s\n", s);
		exit(EXIT_FAILURE);
	}
	return (value * (is_negative ? -1 : 1));
}

size_t
slen(char const * s) {
	size_t len = 0;

	while(s[len]) ++len;
	return (len);
}