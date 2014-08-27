#include <stdlib.h>
#include "containers.h"

void listInit(List *list) {
	list->first = NULL;
	list->last = NULL;
}

void listPush(List *list, void *el) {
	ListNode *node = malloc(sizeof(ListNode));
	node->el = el;
	node->next = NULL;
	
	ListNode *last = list->last;
	node->prev = last;
	if (last) {
		last->next = node;
	}
	else {
		list->first = node;
	}
	list->last = node;
}

void listShift(List *list, void* el) {
	ListNode *node = malloc(sizeof(ListNode));
	node->el = el;
	node->prev = NULL;
	
	ListNode *first = list->first;
	node->next = first;
	if (first) {
		first->prev = node;
	}
	else {
		list->last = node;
	}
	list->first = node;
}

void *listPop(List *list) {
	ListNode *first = list->first;
	if (first) {
		list->first = first->next;
		if (first->next == NULL) {
			list->last = NULL;
		}
		
		void *el = first->el;
		free(first);
		return el;
	}
	else {
		return NULL;
	}
}

void *listTake(List *list) {
	ListNode *last = list->last;
	if (last) {
		list->last = last->prev;
		if (last->prev == NULL) {
			list->first = NULL;
		}
		
		void *el = last->el;
		free(last);
		return el;
	}
	else {
		return NULL;
	}
}

void listClear(List *q) {
	ListNode *node = q->first;
	while (node) {
		ListNode *next = node->next;
		free(node);
		node = next;
	}
}
