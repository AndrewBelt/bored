#pragma once

// Queue
/*
A queue of ints implemented as a singly linked list
*/
typedef struct Node Node;
typedef struct {
	Node *first, *last;
} Queue;

// O(1)
void queuePush(Queue *queue, int val);
// O(1)
// Returns -1 if no elements are in the queue
int queuePop(Queue *queue);
// O(n)
void queueClear(Queue *queue);
// O(n) [could easily be O(1), but I'm lazy]
int queueSize(Queue *queue);



// Priority queue
typedef struct PriqNode PriqNode;
typedef struct {
	PriqNode *buf;
	int n;
	int alloc;
} Priq;

void priqInit(Priq *q, int size);
void priqDestroy(Priq *q);
void priqPush(Priq *q, int val, int pri);
int priqPop(Priq *q);