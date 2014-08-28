#include "bored.h"

struct Gfx gfx;

static Vector sprites[256] = {
	[0x00] = {0, 0},
	[0x01] = {1, 0},
	[0x02] = {2, 0},
};


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

void gfxDrawSprite(uint8_t type, Vector p) {
	Vector sprite = sprites[type];
	SDL_Rect src = {
		sprite.x * TILE_SIZE,
		sprite.y * TILE_SIZE,
		TILE_SIZE, TILE_SIZE
	};
	SDL_Rect dst = {
		gfx.offset.x + p.x*TILE_SIZE*gfx.zoom,
		gfx.offset.y + p.y*TILE_SIZE*gfx.zoom,
		TILE_SIZE*gfx.zoom, TILE_SIZE*gfx.zoom
	};
	
	SDL_RenderCopy(gfx.renderer, gfx.tileTexture, &src, &dst);
}

void gfxDrawRect(Vector p) {
	SDL_Rect dst = {
		gfx.offset.x + p.x*TILE_SIZE*gfx.zoom,
		gfx.offset.y + p.y*TILE_SIZE*gfx.zoom,
		TILE_SIZE*gfx.zoom, TILE_SIZE*gfx.zoom
	};
	SDL_RenderFillRect(gfx.renderer, &dst);
}

void gfxRender() {
	// Clear screen
	SDL_SetRenderDrawBlendMode(gfx.renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(gfx.renderer, 0, 0, 0, 255);
	SDL_RenderClear(gfx.renderer);
	
	// Render map
	Vector p;
	for (p.y = 0; p.y < map.size.y; p.y++) {
		for (p.x = 0; p.x < map.size.x; p.x++) {
			Tile *tile = mapGetTile(p);
			gfxDrawSprite(tile->type, p);
			if (tile->task) {
				// Render task
				SDL_SetRenderDrawColor(gfx.renderer, 0, 255, 0, 128);
				gfxDrawRect(p);
			}
		}
	}
	
	// Render hover
	SDL_SetRenderDrawColor(gfx.renderer, 255, 0, 0, 128);
	gfxDrawRect(map.hover);
	
	// Render player
	for (ListNode *i = map.minions.first; i; i = i->next) {
		Minion *minion = i->el;
		gfxDrawSprite(2, minion->pos);
	}
	
	SDL_RenderPresent(gfx.renderer);
}
