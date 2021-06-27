#ifndef LIST_H
# define LIST_H
# include <unistd.h>
# include <stdlib.h>
# include <stdbool.h>
# include <stdio.h>

typedef struct s_list_element
{
	double						value;
	struct s_list_element	*prev;
	struct s_list_element	*next;
}				t_list_element;

typedef struct s_list
{
	t_list_element		*head;
	t_list_element		*tail;
	size_t				size;
}				t_list;

t_list_element			*create_list_element(double value, t_list_element *prev,
						t_list_element *next);
t_list_element			*list_inject(t_list *list, double value);
void					list_destroy(t_list *list);

void					list_initialize(t_list *list);
t_list_element			*list_push(t_list *list, double data);
double					list_pop(t_list *list);
t_list_element			*list_unshift(t_list *list, double data);
double					list_shift(t_list *list);

t_list_element			*list_get_smallest(t_list const *list);
t_list_element			*list_get_biggest(t_list const *list);

#endif