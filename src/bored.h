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


// generic

typedef struct {
	int x, y;
} Vector;


// Tile

typedef uint8_t Tile;

#define TILE_SIZE 16
// For now odd tiles collide
#define TILE_COLLIDES(tile) ((tile) & 0x1)


// Map

typedef struct Task {
	Vector pos;
} Task;

extern struct Map {
	Vector size;
	Tile *tiles;
	Vector hover;
	Vector player;
	List tasks;
} map;

void mapInit();
void mapDestroy();
Tile *mapGetTile(Vector pos);
/*
Gets the 8 surrounding tiles of a tile
The order is
1 2 3
4 . 5
6 7 8
*/
void mapSurroundingTiles(Tile *center, Tile *surr[8]);
void mapAddTask(Vector pos);


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

extern struct Gfx {
	SDL_Renderer *renderer;
	SDL_Texture *tileTexture;
	Vector offset;
	int zoom;
} gfx;

void gfxInit();
void gfxDestroy();
void gfxRender();
