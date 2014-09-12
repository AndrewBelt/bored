#include "bored.h"

struct Map map;

#define TILE_SIZE 16


void mapInit() {
	map.size = (Vector){256, 256};
	map.tiles = calloc(map.size.x * map.size.y, sizeof(Tile));
	listInit(&map.minions);
	
	map.offset = (Vector){SIDEBAR_WIDTH, 0};
	map.zoom = 2;
}

void mapDestroy() {
	listClear(&map.minions);
	free(map.tiles);
}

/*
Uses diamond-square algorithm to generate heightmap to
generate a float array of size (size.x + 1)*(size.y + 1).
Assumes the size is a square with sides power of two.
The pitch of the array is (size.x + 1).
The caller must free the returned array.
*/
float *generatePlasma(Vector size) {
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
				ml += ((float) rand() / RAND_MAX * 2 - 1) * bs.y / size.y;
				tm += ((float) rand() / RAND_MAX * 2 - 1) * bs.x / size.x;
				
				// Compute midpoint
				float mm = (ml + mr + tm + bm)/4;
				mm += ((float) rand() / RAND_MAX * 2 - 1) * bs.x / size.x;
				
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
	float *heightmap = generatePlasma(map.size);
	
	// Convert the heightmap to tile types
	Vector p;
	for (p.y = 0; p.y < map.size.y; p.y++) {
		for (p.x = 0; p.x < map.size.x; p.x++) {
			float height = heightmap[p.x + (map.size.x + 1) * p.y];
			Tile *tile = mapGetTile(p);
			
			if (height < -0.2) {
				tile->type = 0x02;
			}
			else if (height < -0.15) {
				tile->type = 0x03;
			}
			else if (height < 0.2) {
				tile->type = 0x00;
			}
			else {
				tile->type = 0x01;
			}
		}
	}
	
	free(heightmap);
}

// Generates scatter objects
void mapGenScatter() {
	float *heightmap = generatePlasma(map.size);
	
	Vector p;
	for (p.y = 0; p.y < map.size.y; p.y++) {
		for (p.x = 0; p.x < map.size.x; p.x++) {
			float height = heightmap[p.x + (map.size.x + 1) * p.y];
			Tile *tile = mapGetTile(p);
			
			// Scatter tree tiles
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
	if (0 <= p.x && p.x < map.size.x && 0 <= p.y && p.y < map.size.y) {
		return &map.tiles[p.x + map.size.x * p.y];
	}
	else {
		return NULL;
	}
}

inline Vector mapSelMin() {
	return (Vector){min(map.selStart.x, map.selEnd.x), min(map.selStart.y, map.selEnd.y)};
}

inline Vector mapSelMax() {
	return (Vector){max(map.selStart.x, map.selEnd.x), max(map.selStart.y, map.selEnd.y)};
}

void mapDrawSprite(uint8_t type, Vector p) {
	Vector sprite = {type % 16, type / 16};
	SDL_Rect src = {
		sprite.x * TILE_SIZE,
		sprite.y * TILE_SIZE,
		TILE_SIZE, TILE_SIZE
	};
	SDL_Rect dst = {
		map.offset.x + map.zoom * p.x * TILE_SIZE,
		map.offset.y + map.zoom * p.y * TILE_SIZE,
		map.zoom * TILE_SIZE, map.zoom * TILE_SIZE
	};
	
	SDL_RenderCopy(gfx.renderer, gfx.spritesheet, &src, &dst);
}

void mapDrawRect(Vector p) {
	SDL_Rect dst = {
		map.offset.x + map.zoom * p.x * TILE_SIZE,
		map.offset.y + map.zoom * p.y * TILE_SIZE,
		map.zoom * TILE_SIZE, map.zoom * TILE_SIZE
	};
	SDL_RenderFillRect(gfx.renderer, &dst);
}

void mapRender() {
	// Render tiles
	Vector p;
	for (p.y = 0; p.y < map.size.y; p.y++) {
		for (p.x = 0; p.x < map.size.x; p.x++) {
			Tile *tile = mapGetTile(p);
			mapDrawSprite(tile->type, p);
			if (tile->task) {
				// Render task
				SDL_SetRenderDrawColor(gfx.renderer, 0, 255, 0, 128);
				mapDrawRect(p);
			}
		}
	}
	
	// Render selection
	{
		SDL_SetRenderDrawColor(gfx.renderer, 255, 0, 0, 128);
		Vector selMin = mapSelMin();
		Vector selMax = mapSelMax();
		Vector sel;
		for (sel.y = selMin.y; sel.y <= selMax.y; sel.y++) {
			for (sel.x = selMin.x; sel.x <= selMax.x; sel.x++) {
				mapDrawRect(sel);
			}
		}
	}
	
	// Render minions
	for (ListNode *i = map.minions.first; i; i = i->next) {
		Minion *minion = i->el;
		mapDrawSprite(0xf0, minion->pos);
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

bool mapHandleEvent(SDL_Event *event) {
	switch (event->type) {
	case SDL_MOUSEMOTION:
		// Drag the map with the right mouse button
		if (event->motion.state & SDL_BUTTON_RMASK) {
			map.offset.x += event->motion.x - map.dragPos.x;
			map.offset.y += event->motion.y - map.dragPos.y;
			SDL_WarpMouseInWindow(engine.window, map.dragPos.x, map.dragPos.y);
		}
		
		// Update block selection
		map.selEnd = (Vector){
			eucDiv(event->motion.x - map.offset.x, TILE_SIZE*map.zoom),
			eucDiv(event->motion.y - map.offset.y, TILE_SIZE*map.zoom),
		};
		if (!(event->motion.state & SDL_BUTTON_LMASK)) {
			map.selStart = map.selEnd;
		}
		break;
	
	case SDL_MOUSEBUTTONDOWN:
		if (event->button.button == SDL_BUTTON_RIGHT) {
			// Capture cursor for map dragging
			SDL_GetMouseState(&map.dragPos.x, &map.dragPos.y);
			SDL_ShowCursor(0);
		}
		break;
	
	case SDL_MOUSEBUTTONUP:
		if (event->button.button == SDL_BUTTON_LEFT) {
			// Holding in SHIFT deselects the block
			SDL_Keymod mod = SDL_GetModState();
			if (mod & KMOD_SHIFT) {
				mapDeselect();
			}
			else {
				mapSelect();
			}
			
			map.selStart = map.selEnd;
		}
		else if (event->button.button == SDL_BUTTON_RIGHT) {
			// Uncapture cursor
			SDL_ShowCursor(1);
		}
		break;
		
	case SDL_KEYDOWN:
		if (event->key.keysym.sym == SDLK_1) {
			map.zoom = 1;
		}
		else if (event->key.keysym.sym == SDLK_2) {
			map.zoom = 2;
		}
	}
	
	return true;
}
