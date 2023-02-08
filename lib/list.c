#include "list.h"

List getHead(List list) {
	while (list->prev != NULL) list = list->prev;

	return list;
}

int getSize(List list) {
	int i = 1;
	while (list->next != NULL) {
		list = list->next;
		i++;
	}

	return i;
}

List add(void *element, int index, List list) {
	List new = (List) malloc(sizeof(struct List));
	if (new == NULL) return NULL;
	
	new->element = element;	

	if (list == NULL) {
		new->next = NULL;
		new->prev = NULL;

		list = new;
		return list;
	}

	List curr = list;
	int i = 0;

	while (i != index, curr->next != NULL) {
		curr = curr->next;
		i++;
	}

	new->next = curr;
	new->prev = curr->prev;
	curr->prev = new;

	return list;
}

List remove(int index, List list) {
	int i = 0;
	List save = list;

	while (i != index && list->next != NULL) {
		list = list->next;
		i++;
	}
	if (list->next == NULL && i != index) return NULL;

	remove_exact(list);

	return save;
}

List remove_match(void *element, List list) {
	List save = list;

	while (element != list->element && list->next != NULL) list = list->next;
	if (list->next == NULL && list->element != element) return NULL;

	remove_exact(list);

	return save;
}

int remove_exact(List list) {
	if (list == NULL) return 1;

	list->prev->next = list->next;
	if (list->next != NULL) list->next->prev = list->prev;

	free(list);

	return 0;
}

