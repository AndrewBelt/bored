#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "containers.h"


// Maths

#define PI 3.141592653589793

inline int min(int a, int b) {
	return (a < b) ? a : b;
}
inline int max(int a, int b) {
	return (a > b) ? a : b;
}
inline int eucDiv(int a, int b) {
	return (a - (a<0 ? b-1 : 0)) / b;
}

typedef struct {
	int32_t x, y;
} Vector;

// Tile

typedef struct {
	uint8_t type;
	uint8_t task;
} Tile;

// Map

typedef struct {
	Vector pos;
	Vector task;
	bool hasTask;
} Minion;

extern struct Map {
	Vector size;
	Tile *tiles;
	List minions;
	Vector selStart, selEnd;
} map;

void mapInit();
void mapDestroy();
void mapSeed(uint32_t seed);
Tile *mapGetTile(Vector pos);
inline Vector mapSelMin() {
	return (Vector){min(map.selStart.x, map.selEnd.x), min(map.selStart.y, map.selEnd.y)};
}
inline Vector mapSelMax() {
	return (Vector){max(map.selStart.x, map.selEnd.x), max(map.selStart.y, map.selEnd.y)};
}
// Does nothing if the tile cannot be assigned a task
void mapSelect();
void mapDeselect();


// Engine

extern struct Engine {
	SDL_Window *window;
	bool running;
	int frame;
} engine;

void engineCheckEvent(SDL_Event *event);
void engineRun();
void engineInit();
void engineDestroy();


// Phys

void physStep();


// Gfx
// Draws the map and other stuff

#define TILE_SIZE 16

extern struct Gfx {
	SDL_Renderer *renderer;
	SDL_Texture *spritesheet;
	SDL_Texture *minimap;
	Vector offset;
	int zoom;
	// Actually the inverse zoom
	int minimapZoom;
} gfx;

void gfxInit();
void gfxDestroy();
void gfxRender();
void gfxMinimapRefresh();