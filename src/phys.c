#include "bored.h"

inline int tileToIndex(Tile *tile) {
	return (tile - map.tiles) / sizeof(Tile);
}

void physWalk() {
	ListNode *taskNode = map.tasks.first;
	if (!taskNode) {
		return;
	}
	
	Task *task = taskNode->val;
	
	// Instead of finding a path from the player to the target tile,
	// "pull" the player toward the target by searching a path from
	// the target to the player.
	Tile *start = mapGetTile(task->pos);
	Tile *end = mapGetTile(map.player);
	
	// Initialize priority queue for minimum distance lookup.
	// The map width is a decent starting size.
	Priq q;
	priqInit(&q, map.size.x);
	priqPush(&q, start, 1);
	
	int *dist = calloc(map.size.x * map.size.y, sizeof(int));
	// 0 means infinity, so set the starting distance to 1
	dist[tileToIndex(start)] = 1;
	
	Tile **prev = calloc(map.size.x * map.size.y, sizeof(Tile*));
	prev[tileToIndex(start)] = NULL;
	
	bool *checked = calloc(map.size.x * map.size.y, sizeof(bool));
	
	Tile *u;
	while ((u = priqPop(&q))) {
		// Skip if already reached
		if (checked[tileToIndex(u)])
			continue;
		
		// We now have the unchecked vertex with the minimum distance.
		checked[tileToIndex(u)] = true;
		
		if (u == end) {
			// We have found minion
			break;
		}
		
		Tile *surr[8];
		mapSurroundingTiles(u, surr);
		
		for (int i = 0; i < 8; i++) {
			Tile *v = surr[i];
			if (v && !TILE_COLLIDES(*v)) {
				int d = dist[tileToIndex(u)] + 1;
				if (dist[tileToIndex(v)] == 0 || d < dist[tileToIndex(v)]) {
					// We have found a better path to v
					dist[tileToIndex(v)] = d;
					prev[tileToIndex(v)] = u;
					priqPush(&q, v, d);
				}
			}
		}
	}
	
	// Move player along the last edge in the path
	if (u) {
		Tile *u2 = prev[tileToIndex(u)];
		if (u2) {
			if (u2 == start) {
				listPop(&map.tasks);
			}
			else {
				int index = tileToIndex(u2);
				map.player.x = index % map.size.x;
				map.player.y = index / map.size.y;
			}
		}
	}
	
	free(checked);
	free(prev);
	free(dist);
	priqDestroy(&q);
}

void physStep() {
	physWalk();
}
