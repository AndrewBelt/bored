#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "containers.h"

// Constants

const int TILE_SIZE = 16;

// Types

typedef struct {
	int x;
	int y;
} Vector;

typedef uint8_t Tile;

// Global objects

struct {
	SDL_Window *window;
	bool running;
	int frame;
} engine;

struct {
	SDL_Renderer *renderer;
	SDL_Texture *tileTexture;
	Vector offset;
	int zoom;
} gfx;

struct {
	Vector size;
	Tile *tiles;
	Vector hover;
	Vector player;
	Vector task;
} map;

// map

// Returns a pointer to the tile, or NULL if it is out of bounds
Tile *mapGetTile(int x, int y) {
	if (0 <= x && x < map.size.x && 0 <= y && y < map.size.y) {
		return &map.tiles[x + map.size.x*y];
	}
	else {
		return NULL;
	}
}

bool mapCollides(int x, int y) {
	Tile *tile = mapGetTile(x, y);
	if (tile) {
		return *tile & 0x1;
	}
	else {
		return true;
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
			*mapGetTile(x, y) = (rand() % 3 ? 0 : 1);
		}
	}
	
	map.hover = (Vector){-1, -1};
	map.player = (Vector){0, 0};
	map.task = (Vector){0, 0};
}

void mapDestroy() {
	free(map.tiles);
}

// gfx

void gfxInit() {
	gfx.renderer = SDL_CreateRenderer(engine.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	
	// Load tileset
	SDL_Surface *tileSurface = IMG_Load("tiles.png");
	gfx.tileTexture = SDL_CreateTextureFromSurface(gfx.renderer, tileSurface);
	SDL_FreeSurface(tileSurface);
	
	gfx.offset = (Vector){0, 0};
	gfx.zoom = 2;
}

void gfxDestroy() {
	SDL_DestroyTexture(gfx.tileTexture);
	SDL_DestroyRenderer(gfx.renderer);
}

void gfxDrawTile(Tile tile, int x, int y) {
	SDL_Rect src = {
		(int) tile * TILE_SIZE, 0,
		TILE_SIZE, TILE_SIZE
	};
	SDL_Rect dst = {
		gfx.offset.x + x*TILE_SIZE*gfx.zoom, gfx.offset.y + y*TILE_SIZE*gfx.zoom,
		TILE_SIZE*gfx.zoom, TILE_SIZE*gfx.zoom
	};
	
	SDL_RenderCopy(gfx.renderer, gfx.tileTexture, &src, &dst);
}

void gfxRender() {
	SDL_RenderClear(gfx.renderer);
	
	// Render map
	for (int y = 0; y < map.size.y; y++) {
		for (int x = 0; x < map.size.x; x++) {
			Tile tile = *mapGetTile(x, y);
			bool hover = (x == map.hover.x && y == map.hover.y);
			if (hover) {
				SDL_SetTextureColorMod(gfx.tileTexture, 255, 128, 128);
			}
			gfxDrawTile(tile, x, y);
			if (hover) {
				SDL_SetTextureColorMod(gfx.tileTexture, 255, 255, 255);
			}
		}
	}
	
	// Render player
	gfxDrawTile(2, map.player.x, map.player.y);
	
	SDL_RenderPresent(gfx.renderer);
}

// phys

void physWalk() {
	// TODO
	/*
	This algorithm only verifies that a tile can be reached
	by the player.
	It needs to actually find the minimum path and then
	walk the player by one cell.
	
	The player can walk in diagonals, but not cut corners. (TODO)
	For example, the following move from s to e is invalid:
	|s| |
	|#|e|
	*/
	
	int start = map.task.x + map.size.x * map.task.y;
	int end = map.player.x + map.size.x * map.player.y;
	
	// Create queue and add the start tile
	Queue q;
	memset(&q, 0, sizeof(Queue));
	queuePush(&q, start);
	
	// Create set and add the start tile
	bool *visited = calloc(map.size.x * map.size.y, sizeof(bool));
	visited[start] = true;
	
	int *previous = calloc(map.size.x * map.size.y, sizeof(Tile*));
	previous[start] = -1;
	
	// Start accepting the tiles from the queue
	int id;
	while ((id = queuePop(&q)) >= 0) {
		if (id == end) {
			break;
		}
		
		int tx = id % map.size.x;
		int ty = id / map.size.y;
		
		for (int dy = -1; dy <= 1; dy++) {
			for (int dx = -1; dx <= 1; dx++) {
				if (dx == 0 && dy == 0)
					continue;
				
				int x = tx + dx;
				int y = ty + dy;
				
				if (mapCollides(x, y))
					continue;
				
				// `adjTile` is an adjacent node
				
				int adjId = x + map.size.x * y;
				if (!visited[adjId]) {
					visited[adjId] = true;
					previous[adjId] = id;
					queuePush(&q, adjId);
				}
			}
		}
	}
	
	if (id >= 0) {
		id = previous[id];
		if (id >= 0) {
			int tx = id % map.size.x;
			int ty = id / map.size.y;
			map.player.x = tx;
			map.player.y = ty;
		}
	}
	
	queueClear(&q);
	free(previous);
	free(visited);
}

void physStep() {
	physWalk();
}

// engine

void engineCheckEvent(SDL_Event *event) {
	switch (event->type) {
	case SDL_QUIT:
		engine.running = false;
		break;
	
	case SDL_MOUSEMOTION:
		if (event->motion.state == SDL_BUTTON_RMASK) {
			gfx.offset.x += event->motion.xrel;
			gfx.offset.y += event->motion.yrel;
		}
		map.hover = (Vector){
			(event->motion.x - gfx.offset.x) / (TILE_SIZE*gfx.zoom),
			(event->motion.y - gfx.offset.y) / (TILE_SIZE*gfx.zoom),
		};
		break;
	
	case SDL_MOUSEBUTTONDOWN:
		if (event->button.button == SDL_BUTTON_LEFT) {
			map.task = map.hover;
		}
		break;
	
	case SDL_MOUSEBUTTONUP:
		break;
	
	case SDL_KEYDOWN:
		if (event->key.keysym.sym == SDLK_SPACE) {
			mapDestroy();
			mapInit();
		}
		break;
	}
}

void engineRun() {
	engine.running = true;
	engine.frame = 0;
	while (engine.running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			engineCheckEvent(&event);
		}
		
		if (engine.frame % 8 == 0) {
			physStep();
		}
		gfxRender();
		
		engine.frame++;
	}
}

void engineInit() {
	assert(!SDL_Init(SDL_INIT_VIDEO));
	
	engine.window = SDL_CreateWindow("bored", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, 0);
	assert(engine.window);
	
	gfxInit();
	mapInit();
}

void engineDestroy() {
	mapDestroy();
	gfxDestroy();
	SDL_DestroyWindow(engine.window);
	SDL_Quit();
}

int main() {
	engineInit();
	engineRun();
	engineDestroy();
	return 0;
}