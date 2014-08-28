#include "bored.h"

struct Map map;

void mapInit() {
	map.size = (Vector){64, 64};
	map.tiles = calloc(map.size.x * map.size.y, sizeof(Tile));
	
	listInit(&map.minions);
	
	// TEMP
	/*
	for (int i = 0; i < 10; i++) {
		Minion *minion = calloc(1, sizeof(Minion));
		minion->pos = (Vector){rand()%map.size.x, rand()&map.size.y};
		listPush(&map.minions, minion);
	}
	*/
}

void mapSeed(unsigned int seed) {
	srand(seed);
	#define SEED_TERMS 7
	float xA[SEED_TERMS] = {0};
	float yA[SEED_TERMS] = {0};
	float xP[SEED_TERMS] = {0};
	float yP[SEED_TERMS] = {0};
	
	// Randomize the phase and amplitude in the x and y direction
	for (int i = 1; i < SEED_TERMS; i++) {
		xA[i] = (float) rand() / RAND_MAX * 2 - 1;
		yA[i] = (float) rand() / RAND_MAX * 2 - 1;
		xP[i] = (float) rand() / RAND_MAX;
		yP[i] = (float) rand() / RAND_MAX;
	}
	
	Vector pos;
	for (pos.y = 0; pos.y < map.size.y; pos.y++) {
		for (pos.x = 0; pos.x < map.size.x; pos.x++) {
			float xF = (float) pos.x / map.size.x;
			float yF = (float) pos.y / map.size.y;
			float sum = 0.0;
			
			for (int i = 0; i < SEED_TERMS; i++) {
				sum += xA[i] * sinf(2*PI * (xF*i/5 + xP[i])) / (i+1);
				sum += yA[i] * sinf(2*PI * (yF*i/5 + yP[i])) / (i+1);
			}
			
			Tile *tile = mapGetTile(pos);
			tile->type = (sum > -0.1) ? 0 : 1;
		}
	}
}

void mapDestroy() {
	listClear(&map.minions);
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

void mapSelect() {
	Vector selMin = mapSelMin();
	Vector selMax = mapSelMax();
	Vector sel;
	for (sel.y = selMin.y; sel.y <= selMax.y; sel.y++) {
		for (sel.x = selMin.x; sel.x <= selMax.x; sel.x++) {
			Tile *tile = mapGetTile(sel);
			
			if (tile) {
				// Only select rock
				if (tile->type == 1) {
					tile->task = 255;
				}
			}
		}
	}
}

void mapDeselect() {
	Vector selMin = mapSelMin();
	Vector selMax = mapSelMax();
	Vector sel;
	for (sel.y = selMin.y; sel.y <= selMax.y; sel.y++) {
		for (sel.x = selMin.x; sel.x <= selMax.x; sel.x++) {
			Tile *tile = mapGetTile(sel);
			
			if (tile) {
				tile->task = 0;
			}
		}
	}
}
