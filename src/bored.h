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


// Tile

typedef uint16_t Tile;

// For now odd tiles collide
#define TILE_TYPE(tile) ((tile) & 0xff)
// TODO
// Use a lookup table for slowness weights
#define TILE_COLLIDES(tile) ((tile) & 0x1)
#define TILE_TASK(tile) ((tile) >> 8)

// Map

typedef struct {
	Vector pos;
	Vector task;
	bool hasTask;
} Minion;

extern struct Map {
	Vector size;
	Tile *tiles;
	Vector hover;
	List minions;
} map;

void mapInit();
void mapSeed(unsigned int seed);
void mapDestroy();
Tile *mapGetTile(Vector pos);
// Does nothing if the tile cannot be assigned a task
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

#define TILE_SIZE 16

extern struct Gfx {
	SDL_Renderer *renderer;
	SDL_Texture *tileTexture;
	Vector offset;
	int zoom;
} gfx;

void gfxInit();
void gfxDestroy();
void gfxRender();
