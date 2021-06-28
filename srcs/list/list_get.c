#include "list.h"

double
list_get_smallest(t_list const * list) {
	t_list_element	*smallest;
	t_list_element	*element;

	smallest = list->head;
	element = list->head;
	while (element != list->tail) {
		if (element->value < smallest->value)
			smallest = element;
		element = element->next;
	}
	if (smallest->value < list->tail->value)
		return (smallest->value);
	return (list->tail->value);
}

double
list_get_biggest(t_list const * list) {
	t_list_element	*biggest;
	t_list_element	*element;

	biggest = list->head;
	element = list->head;
	while (element != list->tail) {
		if (element->value > biggest->value)
			biggest = element;
		element = element->next;
	}
	if (biggest->value > list->tail->value)
		return (biggest->value);
	return (list->tail->value);
}

double
list_get_average(t_list const * list) {
	double				total = 0;

	for (t_list_element const * element = list->head;
	element != list->tail; element = element->next)
		total += element->value;
	total += list->tail->value;
	return (total / list->size);
}

static double
absolute(double value) {
	return (value < 0 ? -value : value);
}

double
list_get_mdev(t_list const * list) {
	double				total_dev = 0;
	double				average = list_get_average(list);

	for (t_list_element const * element = list->head;
	element != list->tail; element = element->next)
		total_dev += absolute(element->value - average);
	total_dev += absolute(list->tail->value - average);
	return (total_dev / list->size);
}