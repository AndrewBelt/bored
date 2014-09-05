#include "bored.h"

struct Map map;

void mapInit() {
	map.size = (Vector){256, 256};
	map.tiles = calloc(map.size.x * map.size.y, sizeof(Tile));
	
	listInit(&map.minions);
}

void mapDestroy() {
	listClear(&map.minions);
}

/*
Uses diamond-square algorithm to generate heightmap to
generate a float array of size (size.x + 1)*(size.y + 1).
Assumes the size is a square with sides power of two.
The pitch of the array is (size.x + 1).
The caller must free.
*/
float *mapGenHeightmap(Vector size) {
	assert(size.x == size.y);
	assert((size.x & (size.x - 1)) == 0); // Check power of two
	
	float *h = calloc((size.x + 1) * (size.y + 1), sizeof(float));
	const int pitch = size.x + 1;
	
	// Block size
	Vector bs = size;
	while (bs.x >= 2 && bs.y >= 2) {
		// Block position
		Vector b;
		for (b.y = 0; b.y < size.y; b.y += bs.y) {
			for (b.x = 0; b.x < size.x; b.x += bs.x) {
				// Get four corners of block
				float tl = h[(b.x)        + pitch * (b.y)];
				float tr = h[(b.x + bs.x) + pitch * (b.y)];
				float bl = h[(b.x)        + pitch * (b.y + bs.y)];
				float br = h[(b.x + bs.x) + pitch * (b.y + bs.y)];
				
				// Compute diamond
				float ml = (tl + bl)/2;
				float mr = (tr + br)/2;
				float tm = (tl + tr)/2;
				float bm = (bl + br)/2;
				ml += ((float) rand() / RAND_MAX * 2 - 1) * (bs.x);
				// mr += ((float) rand() / RAND_MAX * 2 - 1) * (bs.x);
				tm += ((float) rand() / RAND_MAX * 2 - 1) * (bs.x);
				// bm += ((float) rand() / RAND_MAX * 2 - 1) * (bs.x);
				
				// Compute midpoint
				float mm = (ml + mr + tm + bm)/4;
				mm += ((float) rand() / RAND_MAX * 2 - 1) * (bs.x);
				
				// Store points
				h[(b.x)          + pitch * (b.y + bs.y/2)] = ml;
				h[(b.x + bs.x)   + pitch * (b.y + bs.y/2)] = mr;
				h[(b.x + bs.x/2) + pitch * (b.y)]          = tm;
				h[(b.x + bs.x/2) + pitch * (b.y + bs.y)]   = bm;
				h[(b.x + bs.x/2) + pitch * (b.y + bs.y/2)] = mm;
			}
		}
		
		bs.x /= 2;
		bs.y /= 2;
	}
	
	return h;
}

void mapGenTerrain() {
	float *heightmap = mapGenHeightmap(map.size);
	
	// Convert the heightmap to tile types
	Vector p;
	for (p.y = 0; p.y < map.size.y; p.y++) {
		for (p.x = 0; p.x < map.size.x; p.x++) {
			float height = heightmap[p.x + (map.size.x + 1) * p.y];
			Tile *tile = mapGetTile(p);
			
			if (height < -40.0) {
				tile->type = 0x02;
			}
			else if (height < -30.0) {
				tile->type = 0x03;
			}
			else if (height < 40.0) {
				tile->type = 0x00;
			}
			else {
				tile->type = 0x01;
			}
		}
	}
	
	free(heightmap);
}

// Generates scatter objects, e.g. trees
void mapGenScatter() {
	float *heightmap = mapGenHeightmap(map.size);
	
	Vector p;
	for (p.y = 0; p.y < map.size.y; p.y++) {
		for (p.x = 0; p.x < map.size.x; p.x++) {
			float height = heightmap[p.x + (map.size.x + 1) * p.y];
			Tile *tile = mapGetTile(p);
			
			// Scatter dirt tiles
			if (tile->type == 0x00) {
				if (height > 0.0 && rand() % 4 == 0) {
					tile->type = 0x10;
				}
			}
		}
	}
}

void mapSeed(uint32_t seed) {
	srand(seed);
	mapGenTerrain();
	mapGenScatter();
	
	// Create some minions
	for (int i = 0; i < 1; i++) {
		Minion *minion = calloc(1, sizeof(Minion));
		minion->pos = (Vector){i, 0};
		listPush(&map.minions, minion);
	}
}

// Returns a pointer to the tile, or NULL if it is out of bounds
Tile *mapGetTile(Vector p) {
	if (0 <= p.x && p.x < map.size.x
		&& 0 <= p.y && p.y < map.size.y) {
		return &map.tiles[p.x + map.size.x * p.y];
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
				tile->task = tileTypes[tile->type].taskType.difficulty;
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
