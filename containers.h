#pragma once

// Queue
/*
A queue implemented as a singly linked list
*/
typedef struct Node Node;
typedef struct {
	Node *first, *last;
} Queue;

void queuePush(Queue *queue, void *element);
void *queuePop(Queue *queue);
void queueClear(Queue *queue);
int queueSize(Queue *queue);
