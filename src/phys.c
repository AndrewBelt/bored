#include "bored.h"


void physWalk() {
	// Instead of finding a path from the player to the target tile,
	// "pull" the player toward the target by searching a path from
	// the target to the player.
	int start = map.task.x + map.size.x * map.task.y;
	int end = map.player.x + map.size.x * map.player.y;
	
	// Initialize priority queue for minimum distance lookup
	Priq q;
	priqInit(&q, map.size.x);
	priqPush(&q, start, 1);
	
	int *dist = calloc(map.size.x * map.size.y, sizeof(int));
	// 0 means infinity, so set the starting distance to 1
	dist[start] = 1;
	
	int *prev = calloc(map.size.x * map.size.y, sizeof(int));
	// -1 means start path
	prev[start] = -1;
	
	bool *checked = calloc(map.size.x * map.size.y, sizeof(bool));
	
	int u;
	while ((u = priqPop(&q)) >= 0) {
		// Skip if already reached
		if (checked[u])
			continue;
		
		// We now have the unchecked vertex with the minimum distance.
		checked[u] = true;
		
		if (u == end)
			break;
		
		int tx = u % map.size.x;
		int ty = u / map.size.y;
		
		// Collect collision info
		int adj[3][3];
		for (int dy = -1; dy <= 1; dy++) {
			for (int dx = -1; dx <= 1; dx++) {
				int x = tx + dx;
				int y = ty + dy;
				Tile *tile = mapGetTile(x, y);
				if (!tile || TILE_COLLIDES(*tile)) {
					adj[dx + 1][dy + 1] = -1;
				}
				else {
					adj[dx + 1][dy + 1] = x + map.size.x * y;
				}
			}
		}
		
		// Restrict to allowed movements
		adj[1][1] = -1;
		if (adj[0][1] < 0) {
			adj[0][0] = adj[0][2] = -1;
		}
		if (adj[2][1] < 0) {
			adj[2][0] = adj[2][2] = -1;
		}
		if (adj[1][0] < 0) {
			adj[0][0] = adj[2][0] = -1;
		}
		if (adj[1][2] < 0) {
			adj[0][2] = adj[2][2] = -1;
		}
		
		// Loop through allowed movements
		for (int dy = -1; dy <= 1; dy++) {
			for (int dx = -1; dx <= 1; dx++) {
				int v = adj[dx + 1][dy + 1];
				if (v < 0)
					continue;
				
				// v is an adjacent vertex to u
				int alt = dist[u] + 1;
				if (dist[v] == 0 || alt < dist[v]) {
					dist[v] = alt;
					prev[v] = u;
					priqPush(&q, v, alt);
				}
			}
		}
	}
	
	// Move player along the last edge in the path
	if (u >= 0) {
		int u2 = prev[u];
		if (u2 >= 0 && u2 != start) {
			map.player.x = u2 % map.size.x;
			map.player.y = u2 / map.size.y;
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
