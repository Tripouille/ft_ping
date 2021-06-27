#include "list.h"

t_list_element	*list_get_smallest(t_list const *list)
{
	t_list_element	*smallest;
	t_list_element	*element;

	smallest = list->head;
	element = list->head;
	while (element != list->tail)
	{
		if (element->value < smallest->value)
			smallest = element;
		element = element->next;
	}
	if (smallest->value < list->tail->value)
		return (smallest);
	return (list->tail);
}

t_list_element	*list_get_biggest(t_list const *list)
{
	t_list_element	*biggest;
	t_list_element	*element;

	biggest = list->head;
	element = list->head;
	while (element != list->tail)
	{
		if (element->value > biggest->value)
			biggest = element;
		element = element->next;
	}
	if (biggest->value > list->tail->value)
		return (biggest);
	return (list->tail);
}
