#include <stdlib.h>
#include "containers.h"

struct Node {
	void *element;
	Node *next;
};

void queuePush(Queue *queue, void *element) {
	Node *node = malloc(sizeof(Node));
	node->element = element;
	node->next = NULL;
	
	Node *last = queue->last;
	if (last) {
		last->next = node;
	}
	else {
		queue->first = node;
	}
	queue->last = node;
}

void *queuePop(Queue *queue) {
	Node *first = queue->first;
	if (first) {
		queue->first = first->next;
		if (queue->first == NULL) {
			queue->last = NULL;
		}
		
		void *element = first->element;
		free(first);
		return element;
	}
	else {
		return NULL;
	}
}

void queueClear(Queue *queue) {
	Node *node = queue->first;
	while (node) {
		Node *nextNode = node->next;
		free(node);
		node = nextNode;
	}
}

int queueSize(Queue *queue) {
	int size = 0;
	Node *node = queue->first;
	while (node) {
		node = node->next;
		size++;
	}
	return size;
}