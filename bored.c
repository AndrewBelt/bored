#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Constants

const int TILE_SIZE = 16;

// Global objects

struct {
	SDL_Window *window;
	bool running;
	int frame;
} engine;

struct {
	SDL_Renderer *renderer;
	SDL_Texture *tileTexture;
	int offsetX, offsetY;
	int zoom;
} gfx;

struct {
	int w, h;
	int *tiles;
	int hoverX, hoverY;
	int playerX, playerY;
	int playerDestX, playerDestY;
} map;

// Functions

// Returns a pointer to the tile, or NULL if it is out of bounds
int *mapGetTile(int x, int y) {
	if (0 <= x && x < map.w && 0 <= y && y < map.h) {
		return &map.tiles[x + map.w*y];
	}
	else {
		return NULL;
	}
}

void mapInit() {
	map.w = 64; map.h = 64;
	map.tiles = malloc(sizeof(int) * map.w*map.h);
	
	for (int y = 0; y < map.h; y++) {
		for (int x = 0; x < map.w; x++) {
			*mapGetTile(x, y) = (rand() % 8 ? 0 : 1);
		}
	}
	map.hoverX = -1; map.hoverY = -1;
	map.playerX = 0; map.playerY = 0;
}

void mapDestroy() {
	free(map.tiles);
}

void gfxInit() {
	gfx.renderer = SDL_CreateRenderer(engine.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	
	// Load tileset
	SDL_Surface *tileSurface = IMG_Load("tiles.png");
	gfx.tileTexture = SDL_CreateTextureFromSurface(gfx.renderer, tileSurface);
	SDL_FreeSurface(tileSurface);
	
	gfx.offsetX = 0; gfx.offsetY = 0;
	gfx.zoom = 2;
}

void gfxDestroy() {
	SDL_DestroyTexture(gfx.tileTexture);
	SDL_DestroyRenderer(gfx.renderer);
}

void gfxDrawTile(int tile, int x, int y) {
	SDL_Rect src = {
		tile*TILE_SIZE, 0,
		TILE_SIZE, TILE_SIZE
	};
	SDL_Rect dst = {
		gfx.offsetX + x*TILE_SIZE*gfx.zoom, gfx.offsetY + y*TILE_SIZE*gfx.zoom,
		TILE_SIZE*gfx.zoom, TILE_SIZE*gfx.zoom
	};
	
	SDL_RenderCopy(gfx.renderer, gfx.tileTexture, &src, &dst);
}

void gfxRender() {
	SDL_RenderClear(gfx.renderer);
	
	// Render map
	for (int y = 0; y < map.h; y++) {
		for (int x = 0; x < map.w; x++) {
			int tile = *mapGetTile(x, y);
			bool hover = (x == map.hoverX && y == map.hoverY);
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
	gfxDrawTile(2, map.playerX, map.playerY);
	
	SDL_RenderPresent(gfx.renderer);
}

void physStep() {
	// TODO
	/*
	The physics timestep currently walks the player toward the player destination,
	ignoring obstacles.
	It should run a breadth-first search on the map from the player location to
	the player destination, and step once in the correct direction on every frame.
	
	The map obstacles can be detected by
	if (*mapGetTile(x, y)) {
		// Collision!
	}
	*/
	
	if (map.playerX < map.playerDestX) {
		map.playerX++;
	}
	else if (map.playerX > map.playerDestX) {
		map.playerX--;
	}
	if (map.playerY < map.playerDestY) {
		map.playerY++;
	}
	else if (map.playerY > map.playerDestY) {
		map.playerY--;
	}
}

void engineCheckEvent(SDL_Event *event) {
	switch (event->type) {
	case SDL_QUIT:
		engine.running = false;
		break;
	
	case SDL_MOUSEMOTION:
		if (event->motion.state == SDL_BUTTON_RMASK) {
			gfx.offsetX += event->motion.xrel;
			gfx.offsetY += event->motion.yrel;
		}
		map.hoverX = (event->motion.x - gfx.offsetX) / (TILE_SIZE*gfx.zoom);
		map.hoverY = (event->motion.y - gfx.offsetY) / (TILE_SIZE*gfx.zoom);
		break;
	
	case SDL_MOUSEBUTTONDOWN:
		if (event->button.button == SDL_BUTTON_LEFT) {
			map.playerDestX = map.hoverX;
			map.playerDestY = map.hoverY;
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