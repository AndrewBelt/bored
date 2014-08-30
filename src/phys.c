#include "bored.h"

#define VECT2INDEX(v) (v.x + map.size.x * v.y)
#define INDEX2VECT(i) (Vector){i % map.size.x, i / map.size.x}

// Inverse speeds for walking.
// Measures the difficulty of walking on a particular tile.
// 0 means infinity, or collides
// TODO
// Actually support different speed weights for minions
int weights[256] = {
	[0x00] = 1,
	[0x03] = 4,
};

void minionWalk(Minion *minion) {
	// TODO
	// Allocate these statically or something.
	int *prev = calloc(map.size.x * map.size.y, sizeof(int));
	float *dist = calloc(map.size.x * map.size.y, sizeof(float));
	bool *checked = calloc(map.size.x * map.size.y, sizeof(bool));
	
	int startIndex = VECT2INDEX(minion->pos);
	dist[startIndex] = 1;
	
	// Initialize priority queue for minimum distance lookup.
	// The map width is a decent starting size.
	Priq q;
	priqInit(&q);
	priqPush(&q, startIndex, 0);
	
	int endIndex = -1;
	int uIndex;
	while (priqPop(&q, &uIndex)) {
		// Skip if already reached
		if (checked[uIndex]) {
			continue;
		}
		// We now have the unchecked vertex with the minimum distance.
		checked[uIndex] = true;
		
		Vector u = INDEX2VECT(uIndex);
		Tile *uTile = mapGetTile(u);
		if (uTile->task) {
			// We have found a task
			endIndex = uIndex;
			break;
		}
		
		for (int dy = -1; dy <= 1; dy++) {
			for (int dx = -1; dx <= 1; dx++) {
				Vector v = {u.x + dx, u.y + dy};
				Tile *vTile = mapGetTile(v);
				
				if (!vTile) {
					continue;
				}
				
				// TODO
				// Actual collisions
				
				int weight = (vTile->task) ? 1 : weights[vTile->type];
				if (weight == 0) {
					continue;
				}
				
				// Reduce random diagonal movement by forcing L_2 distance
				weight *= (abs(dx) + abs(dy) == 2) ? 181 : 128;
				
				int vIndex = VECT2INDEX(v);
				int d = dist[uIndex] + weight;
				if (dist[vIndex] == 0 || d < dist[vIndex]) {
					// We have found a better path to v
					dist[vIndex] = d;
					prev[vIndex] = uIndex;
					priqPush(&q, vIndex, d);
				}
			}
		}
	}
	
	priqDestroy(&q);
	free(checked);
	free(dist);
	
	if (endIndex >= 0) {
		// Rewind path back to start
		int wIndex = uIndex;
		while (uIndex != startIndex) {
			wIndex = uIndex;
			uIndex = prev[uIndex];
		}
		
		Vector w = INDEX2VECT(wIndex);
		if (wIndex == endIndex) {
			// Complete task
			// TODO
			// Outsource this to another function
			Tile *taskTile = mapGetTile(w);
			taskTile->task = 0;
			taskTile->type = 0x00;
		}
		else {
			// Walk toward the task
			minion->pos = w;
		}
	}
	
	free(prev);
}

void physStep() {
	for (ListNode *i = map.minions.first; i; i = i->next) {
		minionWalk(i->el);
	}
}
