#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "containers.h"


// Math

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


// engine.c

extern struct Engine {
	SDL_Window *window;
	bool running;
	int frame;
} engine;

void engineInit();
void engineDestroy();
void engineRun();


// gfx.c

typedef struct {
	uint8_t r, g, b, a;
} Pixel;

extern struct Gfx {
	SDL_Renderer *renderer;
	TTF_Font *mainFont;
	SDL_Texture *spritesheet;
} gfx;

void gfxInit();
void gfxDestroy();
void gfxRender();


// tile.c

typedef struct {
	uint8_t type;
	uint8_t task;
} Tile;

extern struct TileType {
	// 0 means collides, 1 means fastest to walk on
	int walkWeight;
	struct TaskType {
		// 0 means a task cannot be set for this tile
		uint8_t difficulty;
		uint8_t goalTile;
	} taskType;
} tileTypes[256];


// map.c

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
	Vector offset;
	int zoom;
} map;

void mapInit();
void mapDestroy();
void mapSeed(uint32_t seed);
Tile *mapGetTile(Vector pos);
void mapRender();
bool mapHandleEvent(SDL_Event *event);


// phys.c

void physStep();


// gui.c

#define SIDEBAR_WIDTH 200

extern struct Gui {
	SDL_Texture *minimap;
} gui;

void guiInit();
void guiDestroy();
void guiRender();
bool guiHandleEvent(SDL_Event *event);
