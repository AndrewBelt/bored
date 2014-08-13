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

// For now odd tiles collide
#define TILE_COLLIDES(tile) ((tile) & 0x1)

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
	The player can walk in diagonals, but not cut corners.
	For example, the following move from s to e is invalid:
	|s| |
	|#|e|
	*/
	
	int start = map.player.x + map.size.x * map.player.y;
	int end = map.task.x + map.size.x * map.task.y;
	
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
	
	// Rewind the path back to the start
	int u1 = -1;
	int u2 = -1;
	while (u >= 0) {
		u2 = u1;
		u1 = u;
		u = prev[u];
	}
	
	if (u2 >= 0) {
		map.player.x = u2 % map.size.x;
		map.player.y = u2 / map.size.y;
	}
	
	free(checked);
	free(prev);
	free(dist);
	priqDestroy(&q);
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