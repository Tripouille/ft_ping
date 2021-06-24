NAME	= ft_ping
SRCS	= $(addprefix srcs/, main.c utils.c options.c)
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

db:
	docker build -t mi .

dr:
	docker run -it --rm -v $(pwd):/project -w /project --name mc mi zsh

.PHONY: all clean fclean re db dr
-include $(DEPS)