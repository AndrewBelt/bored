#include "bored.h"

// Conversion macros between a Vector and the index 
#define VECT2INDEX(v) (v.x + map.size.x * v.y)
#define INDEX2VECT(i) (Vector){i % map.size.x, i / map.size.x}


// Advances the progress of a task
void taskStep(Vector p) {
	Tile *tile = mapGetTile(p);
	assert(tile->task > 0);
	
	// Work on task
	tile->task--;
	
	if (tile->task == 0) {
		// Complete task
		tile->type = tileTypes[tile->type].taskType.goalTile;
	}
}


// Starting with the minion, use Dijkstra's algorithm to
// search for a tile with a task.
// FIXME
// Only works with one minion
// This can be solved if minions have a `task` field which
// identifies the task they are dedicated to.
// If a minion has a task, he should only search for that task.
// If he fails to find it, he should be unassigned the task.
// If the minion does not have a task, he should search
// for only tasks which do not have a minion assigned to that
// task.
// (This will require a linear search in the `map.minion` list.)
void minionWalk(Minion *minion) {
	// TODO
	// Allocate these statically or something.
	// This seems like a lot to allocate for a single minion step,
	// but it has constant read/write access.
	// Is there a better way to do it?
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
				
				/*
				FIXME
				The following move is illegal, but this
				algorithm allows it.
				
				. # .
				# o .
				. . .
				
				o # .
				# . .
				. . .
				
				Basically we can't go through corners, so this case
				should be explicitly handled.
				*/
				
				int weight = 1;
				if (vTile->task == 0) {
					weight = tileTypes[vTile->type].walkWeight;
				}
				if (weight == 0) {
					continue;
				}
				
				// Reduce random diagonal movement by forcing L_2 distance
				weight *= (abs(dx) + abs(dy) == 2) ? 141 : 100;
				
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
	
	// Dijkstra's algorithm finished, so let's see what we can do
	// with the results.
	if (endIndex >= 0) {
		// Rewind path back to second node in the path
		int wIndex = uIndex;
		while (uIndex != startIndex) {
			wIndex = uIndex;
			uIndex = prev[uIndex];
		}
		
		Vector w = INDEX2VECT(wIndex);
		if (wIndex == endIndex) {
			taskStep(w);
		}
		else {
			// Walk toward the task
			minion->pos = w;
		}
	}
	
	free(prev);
}

void physStep() {
	// Step each minion
	for (ListNode *i = map.minions.first; i; i = i->next) {
		minionWalk(i->el);
	}
	
	// TODO
	// Add forest fires!
}
