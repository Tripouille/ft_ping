#include "list.h"

void
list_initialize(t_list *list) {
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
}

t_list_element	*
list_push(t_list *list, t_packet_tracker data) {
	t_list_element	*injected_element;

	injected_element = list_inject(list, data);
	if (injected_element == NULL)
		return (NULL);
	list->tail = injected_element;
	return (injected_element);
}

t_packet_tracker
list_pop(t_list *list) {
	t_list_element	*element_to_destroy;
	t_packet_tracker				return_data;

	element_to_destroy = list->tail;
	if (list->head == list->tail) {
		list->head = NULL;
		list->tail = NULL;
	}
	else {
		list->tail->prev->next = list->tail->next;
		list->tail->next->prev = list->tail->prev;
		list->tail = list->tail->prev;
	}
	return_data = element_to_destroy->data;
	free(element_to_destroy);
	list->size -= 1;
	return (return_data);
}

t_list_element	*
list_unshift(t_list *list, t_packet_tracker data) {
	t_list_element	*injected_element;

	injected_element = list_inject(list, data);
	if (injected_element == NULL)
		return (NULL);
	list->head = injected_element;
	return (injected_element);
}

t_packet_tracker
list_shift(t_list *list) {
	t_list_element	*element_to_destroy;
	t_packet_tracker				return_data;

	element_to_destroy = list->head;
	if (list->head == list->tail) {
		list->head = NULL;
		list->tail = NULL;
	}
	else {
		list->head->next->prev = list->head->prev;
		list->head->prev->next = list->head->next;
		list->head = list->head->next;
	}
	return_data = element_to_destroy->data;
	free(element_to_destroy);
	list->size -= 1;
	return (return_data);
}
