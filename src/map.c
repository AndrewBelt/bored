#include "bored.h"

struct Map map;

void mapInit() {
	map.size = (Vector){64, 64};
	map.tiles = malloc(sizeof(Tile) * map.size.x * map.size.y);
	
	Vector pos;
	for (pos.y = 0; pos.y < map.size.y; pos.y++) {
		for (pos.x = 0; pos.x < map.size.x; pos.x++) {
			*mapGetTile(pos) = (rand() % 4 ? 0 : 1);
		}
	}
	
	map.hover = (Vector){-1, -1};
	map.player = (Vector){0, 0};
	listInit(&map.tasks);
}

void mapDestroy() {
	free(map.tiles);
}

// Returns a pointer to the tile, or NULL if it is out of bounds
Tile *mapGetTile(Vector pos) {
	if (0 <= pos.x && pos.x < map.size.x
		&& 0 <= pos.y && pos.y < map.size.y) {
		return &map.tiles[pos.x + map.size.x * pos.y];
	}
	else {
		return NULL;
	}
}

void mapSurroundingTiles(Tile *center, Tile *surr[]) {
	int index = (center - map.tiles) / sizeof(Tile);
	Vector pos = {index % map.size.x, index / map.size.y};
	int i = 0;
	for (int y = -1; y <= 1; y++) {
		for (int x = -1; x <= 1; x++) {
			if (x == 0 && y == 0)
				continue;
			Vector p = pos;
			p.x += x;
			p.y += y;
			surr[i++] = mapGetTile(p);
		}
	}
}

void mapAddTask(Vector pos) {
	// Silently fail if a task is already at this position
	ListNode *taskNode = map.tasks.first;
	while (taskNode) {
		Task *task = taskNode->val;
		if (task->pos.x == pos.x && task->pos.y == pos.y) {
			return;
		}
		taskNode = taskNode->next;
	}
	
	// Create a task and put it on the tasks queue
	Task *task = malloc(sizeof(task));
	task->pos = map.hover;
	listPush(&map.tasks, task);
}
