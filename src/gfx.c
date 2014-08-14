#include "bored.h"

struct Gfx gfx;

static Vector tileSprites[] = {
	{0, 0}, {1, 0}, {2, 0},
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

void gfxDrawTile(Tile tile, int x, int y) {
	Vector tileSprite = tileSprites[tile];
	SDL_Rect src = {
		tileSprite.x * TILE_SIZE, tileSprite.y * TILE_SIZE,
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
