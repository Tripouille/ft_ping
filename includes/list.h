#ifndef LIST_H
# define LIST_H
# include <unistd.h>
# include <stdlib.h>
# include <stdbool.h>
# include <stdio.h>

typedef struct	s_packet_tracker
{
	int				sequence;
	bool			received;
	struct timeval	sent_timeval;
	double			travel_time;
}				t_packet_tracker;

typedef struct s_list_element
{
	t_packet_tracker		data;
	struct s_list_element	*prev;
	struct s_list_element	*next;
}				t_list_element;

typedef struct s_list
{
	t_list_element		*head;
	t_list_element		*tail;
	size_t				size;
}				t_list;

t_list_element			*create_list_element(t_packet_tracker data, t_list_element *prev,
						t_list_element *next);
t_list_element			*list_inject(t_list *list, t_packet_tracker data);
void					list_destroy(t_list *list);

void					list_initialize(t_list *list);
t_list_element			*list_push(t_list *list, t_packet_tracker data);
t_packet_tracker		list_pop(t_list *list);
t_list_element			*list_unshift(t_list *list, t_packet_tracker data);
t_packet_tracker		list_shift(t_list *list);

double						list_get_smallest(t_list const * list);
double						list_get_biggest(t_list const * list);
double						list_get_average(t_list const * list);
double						list_get_mdev(t_list const * list);
t_packet_tracker *			list_get_tracker(t_list * list, int sequence);

#endif