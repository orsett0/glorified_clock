#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

/*
 * I think i can call this a "reverse linked list", couse i'm going to create the "classic"
 * list with a pointer to the head, but i'll also give
 */

typedef struct List {
	/*
	 * The element pointed by the list.
	 * It's a pointer to void so it can be whatever it wants to be.
	 */
	void *element;

	struct List *next;
	struct List *prev;
} * List;

// Returns the head of the list
List getHead(List);

// Return the size of the list
int getSize(List list);

// Adds an element at position index and return the head of the list.
// If list is null, create a new one
List add(void *, int, List);

// Remove the element at position index and returns the head.
List remove(int, List);

// Remove the first occourence of "value" from the list hand returns the head.
List remove_match(void *, List);

// Remove the exact element passed as a parameter from the list
int remove_exact(List element);

#endif
