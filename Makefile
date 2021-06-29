NAME	= ft_ping
SRCS	= $(addprefix srcs/, main.c utils.c options.c initialize.c parsing.c \
			$(addprefix list/, list.c list_get.c list_utils.c)) 
OBJS	= $(SRCS:srcs/%.c=objs/%.o)
DEPS	= $(SRCS:srcs/%.c=deps/%.d)

CC		= gcc
CFLAGS	= -Wall -Werror -Wextra -g3 -Iincludes
DFLAGS	= -MT $@ -MMD -MP -MF deps/$*.d

all: $(NAME)

$(OBJS): objs/%.o: srcs/%.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $^ -o $(NAME)

clean:
	rm -rf $(OBJS) $(DEPS) *.dSYM

fclean: clean
	rm -rf $(NAME)

re: fclean all

bcon:
	@sysctl net.ipv4.icmp_echo_ignore_broadcasts=0

bcoff:
	@sysctl net.ipv4.icmp_echo_ignore_broadcasts=1

.PHONY: all clean fclean re bcon bcoff
-include $(DEPS)