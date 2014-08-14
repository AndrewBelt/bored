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


// types

typedef struct {
	int x;
	int y;
} Vector;

// Tile

typedef uint8_t Tile;
#define TILE_SIZE 16
// For now odd tiles collide
#define TILE_COLLIDES(tile) ((tile) & 0x1)

// Map

extern struct Map {
	Vector size;
	Tile *tiles;
	Vector hover;
	Vector player;
	Vector task;
} map;

Tile *mapGetTile(int x, int y);
Vector mapGetPos(Tile *tile);
void mapInit();
void mapDestroy();

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

extern struct Gfx {
	SDL_Renderer *renderer;
	SDL_Texture *tileTexture;
	Vector offset;
	int zoom;
} gfx;

void gfxInit();
void gfxDestroy();
void gfxRender();
