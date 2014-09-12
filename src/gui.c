#include "bored.h"

struct Gui gui;


Pixel tileColors[256] = {
	[0x00] = {109, 170, 44},
	[0x01] = {211, 125, 44},
	[0x02] = {109, 195, 203},
	[0x03] = {211, 170, 154},
	[0x10] = {52, 101, 36},
};


void guiInit() {
	// FIXME
	// Why is the byte order of the pixel format reversed??
	gui.minimap = SDL_CreateTexture(gfx.renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, map.size.x, map.size.y);
	assert(gui.minimap);
	gui.minimapZoom = 2;
}

void guiDestroy() {
	SDL_DestroyTexture(gui.minimap);
}

void guiMinimapRefresh() {
	Pixel *pixels;
	int pitch;
	
	int err = SDL_LockTexture(gui.minimap, NULL, (void**) &pixels, &pitch);
	assert(!err);
	assert(pitch / sizeof(Pixel) == map.size.x);
	
	// Copy map to individual pixels on the minimap
	Vector p;
	for (p.y = 0; p.y < map.size.y; p.y++) {
		for (p.x = 0; p.x < map.size.x; p.x++) {
			Tile *tile = mapGetTile(p);
			assert(tile);
			pixels[p.x + map.size.x * p.y] = tileColors[tile->type];
			pixels[p.x + map.size.x * p.y].a = 255;
		}
	}
	
	SDL_UnlockTexture(gui.minimap);
}

void guiRender() {
	// Render minimap
	{
		if (engine.frame % 64 == 0) {
			guiMinimapRefresh();
		}
		
		SDL_SetTextureBlendMode(gui.minimap, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(gui.minimap, 255 * 0.9);
		SDL_Rect dst = {
			TILE_SIZE, TILE_SIZE,
			map.size.x/gui.minimapZoom, map.size.y/gui.minimapZoom
		};
		SDL_RenderCopy(gfx.renderer, gui.minimap, NULL, &dst);
	}
	
	// Render font
	{
		/*
		SDL_Surface *fontSurface = TTF_RenderUTF8_Shaded(gfx.mainFont, "Hello world", (SDL_Color){0, 0, 0}, (SDL_Color){255, 255, 255});
		SDL_Texture *fontTexture = SDL_CreateTextureFromSurface(gfx.renderer, fontSurface);
		SDL_RenderCopy(gfx.renderer, fontTexture, NULL, NULL);
		SDL_DestroyTexture(fontTexture);
		SDL_FreeSurface(fontSurface);
		*/
	}
}

bool guiHandleEvent(SDL_Event *event) {
	return false;
}
