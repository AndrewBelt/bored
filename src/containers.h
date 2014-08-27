#pragma once
#include <stdbool.h>
#include <stdint.h>

/*
Philosophy:
Encapsulation is cumbersome, so all structs are exposed.
However, it is recommended to treat all struct members as read-only.
*/

// 2D Vector

typedef struct {
	int x, y;
} Vector;


// List
/*
A singly-linked list of void pointers
*/
typedef struct ListNode ListNode;
struct ListNode {
	void *el;
	ListNode *next, *prev;
};

typedef struct {
	ListNode *first, *last;
} List;

void listInit(List *list);
// O(1)
void listPush(List *list, void *el);
void listShift(List *list, void *el);
// O(1)
// Returns NULL if no elements are in the list
void *listPop(List *list);
void *listTake(List *list);
// O(n)
void listClear(List *list);


// Stack
typedef struct {
	// TODO
} Stack;

void stackInit(Stack *stack);
void stackPush(Stack *stack, void *el);
void *stackPop(Stack *stack);


// Priority queue of Vector elements
typedef struct {
	Vector el;
	int pri;
} PriqNode;

typedef struct {
	PriqNode *buf;
	int n;
	int alloc;
} Priq;

void priqInit(Priq *q);
void priqDestroy(Priq *q);
void priqPush(Priq *q, Vector el, int pri);
// Returns true if successful
bool priqPop(Priq *q, Vector *el);
