#include "list.h"

double
list_get_smallest(t_list const * list) {
	t_list_element	*smallest;
	t_list_element	*element;

	smallest = list->head;
	element = list->head;
	while (element != list->tail) {
		if (element->data.received
		&& element->data.travel_time < smallest->data.travel_time)
			smallest = element;
		element = element->next;
	}
	if (list->tail->data.received
	&& list->tail->data.travel_time < smallest->data.travel_time)
		return (list->tail->data.travel_time);
	return (smallest->data.travel_time);
}

double
list_get_biggest(t_list const * list) {
	t_list_element	*biggest;
	t_list_element	*element;

	biggest = list->head;
	element = list->head;
	while (element != list->tail) {
		if (element->data.received
		&& element->data.travel_time > biggest->data.travel_time)
			biggest = element;
		element = element->next;
	}
	if (list->tail->data.received
	&& list->tail->data.received > biggest->data.travel_time)
		return (list->tail->data.travel_time);
	return (biggest->data.travel_time);
}

double
list_get_average(t_list const * list) {
	double				total = 0;

	for (t_list_element const * element = list->head;
	element != list->tail; element = element->next)
		if (element->data.received) total += element->data.travel_time;
	if (list->tail->data.received) total += list->tail->data.travel_time;
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
		if (element->data.received) total_dev += absolute(element->data.travel_time - average);
	if (list->tail->data.received) total_dev += absolute(list->tail->data.travel_time - average);
	return (total_dev / list->size);
}

t_packet_tracker *
list_get_tracker(t_list * list, int sequence) {
	for (t_list_element * element = list->head;
	element != list->tail; element = element->next)
		if (element->data.sequence == sequence) return (&element->data);
	if (list->tail->data.sequence == sequence) return (&list->tail->data);
	return (NULL);
}