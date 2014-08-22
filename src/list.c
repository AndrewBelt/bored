#include <stdlib.h>
#include "containers.h"

void listInit(List *list) {
	list->first = NULL;
	list->last = NULL;
}

void listPush(List *list, void *val) {
	ListNode *node = malloc(sizeof(ListNode));
	node->val = val;
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

void listShift(List *list, void* val) {
	ListNode *node = malloc(sizeof(ListNode));
	node->val = val;
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
		
		void *val = first->val;
		free(first);
		return val;
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
		
		void *val = last->val;
		free(last);
		return val;
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
