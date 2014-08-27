#include "bored.h"

#define VECT2INDEX(v) (v.x + map.size.x * v.y)

// TODO
// Everything 
void minionWalk(Minion *minion) {
	Vector start = minion->pos;
	
	// TODO
	// Allocate this statically or something.
	
	Vector *prev = calloc(map.size.x * map.size.y, sizeof(Vector));
	
	int *dist = calloc(map.size.x * map.size.y, sizeof(int));
	dist[VECT2INDEX(start)] = 1;
	bool *checked = calloc(map.size.x * map.size.y, sizeof(bool));
	
	// Initialize priority queue for minimum distance lookup.
	// The map width is a decent starting size.
	Priq q;
	priqInit(&q);
	priqPush(&q, start, 0);
	
	bool found = false;
	Vector u;
	while (priqPop(&q, &u)) {
		// Skip if already reached
		if (checked[VECT2INDEX(u)])
			continue;
		// We now have the unchecked vertex with the minimum distance.
		checked[VECT2INDEX(u)] = true;
		
		Tile *uTile = mapGetTile(u);
		if (TILE_TASK(*uTile)) {
			// We have found a task
			found = true;
			break;
		}
		
		for (int dy = -1; dy <= 1; dy++) {
			for (int dx = -1; dx <= 1; dx++) {
				// TODO
				// Actual collisions
				
				Vector v = {u.x + dx, u.y + dy};
				Tile *vTile = mapGetTile(v);
				
				if (!vTile) {
					continue;
				}
				
				int d = dist[VECT2INDEX(u)] + 1;
				if (dist[VECT2INDEX(v)] == 0 || d < dist[VECT2INDEX(v)]) {
					// We have found a better path to v
					dist[VECT2INDEX(v)] = d;
					prev[VECT2INDEX(v)] = u;
					priqPush(&q, v, d);
				}
			}
		}
	}
	
	priqDestroy(&q);
	free(checked);
	free(dist);
	
	// Rewind path to start
	if (found) {
		Vector w = u;
		while (!(u.x == start.x && u.y == start.y)) {
			w = u;
			u = prev[VECT2INDEX(u)];
		}
		
		Tile *taskTile = mapGetTile(w);
		if (TILE_TASK(*taskTile)) {
			*taskTile = *taskTile && 0xff;
		}
		else {
			minion->pos = w;
		}
	}
	
	free(prev);
}

void physStep() {
	/*
	for (ListNode *i = map.minions.first; i; i = i->next) {
		minionWalk(i->el);
	}
	*/
}
