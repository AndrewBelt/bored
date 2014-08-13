#include "containers.h"
#include <stdlib.h>


/*
Implementation derived from
http://rosettacode.org/wiki/Priority_queue#C
under the GNU Free Documentation License
*/


struct PriqNode {
	int val;
	int pri;
};


/* first element in array not used to simplify indices */
void priqInit(Priq *q, int size) {
	if (size < 4) {
		size = 4;
	}
	
	q->buf = malloc(sizeof(PriqNode) * size);
	q->alloc = size;
	q->n = 1;
}

void priqDestroy(Priq *q) {
	free(q->buf);
}

void priqPush(Priq *q, int val, int pri) {
	PriqNode *b;
	int n, m;
	
	if (q->n >= q->alloc) {
		q->alloc *= 2;
		b = q->buf = realloc(q->buf, sizeof(PriqNode) * q->alloc);
	}
	else {
		b = q->buf;
	}
	
	n = q->n++;
	/* append at end, then up heap */
	while ((m = n / 2) && pri < b[m].pri) {
		b[n] = b[m];
		n = m;
	}
	b[n].val = val;
	b[n].pri = pri;
}
	
/* remove top item. returns 0 if empty. *pri can be null. */
int priqPop(Priq *q)
{
	if (q->n == 1) {
		// Return -1 if nothing is in the queue
		return -1;
	}
	
	PriqNode *b = q->buf;
	
	int val = b[1].val;
	
	/* pull last item to top, then down heap. */
	q->n--;
	
	int n = 1;
	int m;
	while ((m = n * 2) < q->n) {
		if (m + 1 < q->n && b[m].pri > b[m + 1].pri)
			m++;
		
		if (b[q->n].pri <= b[m].pri)
			break;
		b[n] = b[m];
		n = m;
	}
	
	b[n] = b[q->n];
	if (q->n < q->alloc / 2 && q->n >= 16) {
		q->buf = realloc(q->buf, (q->alloc /= 2) * sizeof(PriqNode));
	}
	
	return val;
}