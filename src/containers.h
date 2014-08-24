#pragma once

/*
Philosophy:
Encapsulation is cumbersome, so all structs are exposed.
However, it is recommended to treat all struct members as read-only.
*/


// List
/*
A singly-linked list of void pointers
*/
typedef struct ListNode ListNode;
struct ListNode {
	void *val;
	ListNode *next, *prev;
};

typedef struct {
	ListNode *first, *last;
} List;

void listInit(List *list);
// O(1)
void listPush(List *list, void *val);
void listShift(List *list, void* val);
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

void stackInit();
void stackPush(void *val);
void *stackPop();


// Priority queue
typedef struct {
	void *val;
	int pri;
} PriqNode;

typedef struct {
	PriqNode *buf;
	int n;
	int alloc;
} Priq;

void priqInit(Priq *q, int size);
void priqDestroy(Priq *q);
void priqPush(Priq *q, void *val, int pri);
void *priqPop(Priq *q);