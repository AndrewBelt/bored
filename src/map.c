#include "bored.h"

struct Map map;

// Returns a pointer to the tile, or NULL if it is out of bounds
Tile *mapGetTile(int x, int y) {
	if (0 <= x && x < map.size.x && 0 <= y && y < map.size.y) {
		return &map.tiles[x + map.size.x*y];
	}
	else {
		return NULL;
	}
}

// Converts a tile pointer to its position on the map
Vector mapGetPos(Tile *tile) {
	int index = (tile - map.tiles) / sizeof(Tile);
	return (Vector){index % map.size.x, index / map.size.y};
}

void mapInit() {
	map.size = (Vector){64, 64};
	map.tiles = malloc(sizeof(Tile) * map.size.x * map.size.y);
	
	for (int y = 0; y < map.size.y; y++) {
		for (int x = 0; x < map.size.x; x++) {
			*mapGetTile(x, y) = (rand() % 4 ? 0 : 1);
		}
	}
	
	map.hover = (Vector){-1, -1};
	map.player = (Vector){0, 0};
	map.task = (Vector){0, 0};
}

void mapDestroy() {
	free(map.tiles);
}
