#include "bored.h"

struct Gfx gfx;


void gfxInit() {
	// Create rendering context (software or OpenGL context)
	gfx.renderer = SDL_CreateRenderer(engine.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	assert(gfx.renderer);
	
	// Load font
	gfx.mainFont = TTF_OpenFont("res/DejaVuSans.ttf", 16);
	assert(gfx.mainFont);
	
	// Load tileset
	SDL_Surface *spritesheetSurf = IMG_Load("res/tiles.png");
	assert(spritesheetSurf);
	gfx.spritesheet = SDL_CreateTextureFromSurface(gfx.renderer, spritesheetSurf);
	assert(gfx.spritesheet);
	SDL_FreeSurface(spritesheetSurf);
}

void gfxDestroy() {
	SDL_DestroyTexture(gfx.spritesheet);
	TTF_CloseFont(gfx.mainFont);
	SDL_DestroyRenderer(gfx.renderer);
}

void gfxRender() {
	// Clear screen
	SDL_SetRenderDrawBlendMode(gfx.renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(gfx.renderer, 0, 0, 0, 255);
	SDL_RenderClear(gfx.renderer);
	
	mapRender();
	guiRender();
	
	// Flip buffer
	SDL_RenderPresent(gfx.renderer);
}
