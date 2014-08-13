#include <stdlib.h>
#include "containers.h"

struct Node {
	int val;
	Node *next;
};

void queuePush(Queue *queue, int val) {
	Node *node = malloc(sizeof(Node));
	node->val = val;
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

int queuePop(Queue *queue) {
	Node *first = queue->first;
	if (first) {
		queue->first = first->next;
		if (queue->first == NULL) {
			queue->last = NULL;
		}
		
		int val = first->val;
		free(first);
		return val;
	}
	else {
		return -1;
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