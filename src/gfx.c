#include "bored.h"

struct Gfx gfx;


typedef struct {
	uint8_t r, g, b, a;
} Pixel;

Pixel tileColors[256] = {
	[0x00] = {109, 170, 44},
	[0x01] = {211, 125, 44},
	[0x02] = {109, 195, 203},
	[0x03] = {211, 170, 154},
	[0x10] = {52, 101, 36},
};


void gfxInit() {
	gfx.renderer = SDL_CreateRenderer(engine.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	
	// Load tileset
	SDL_Surface *spritesheetSurf = IMG_Load("tiles.png");
	gfx.spritesheet = SDL_CreateTextureFromSurface(gfx.renderer, spritesheetSurf);
	SDL_FreeSurface(spritesheetSurf);
	
	// TODO
	// Why is the byte order of the pixel format reversed??
	gfx.minimap = SDL_CreateTexture(gfx.renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, map.size.x, map.size.y);
	
	gfx.offset = (Vector){0, 0};
	gfx.zoom = 2;
	gfx.minimapZoom = 2;
}

void gfxDestroy() {
	SDL_DestroyTexture(gfx.minimap);
	SDL_DestroyTexture(gfx.spritesheet);
	SDL_DestroyRenderer(gfx.renderer);
}

void gfxDrawSprite(uint8_t type, Vector p) {
	Vector sprite = {type % 16, type / 16};
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
	
	SDL_RenderCopy(gfx.renderer, gfx.spritesheet, &src, &dst);
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
	
	// Render selection
	{
		SDL_SetRenderDrawColor(gfx.renderer, 255, 0, 0, 128);
		Vector selMin = mapSelMin();
		Vector selMax = mapSelMax();
		Vector sel;
		for (sel.y = selMin.y; sel.y <= selMax.y; sel.y++) {
			for (sel.x = selMin.x; sel.x <= selMax.x; sel.x++) {
				gfxDrawRect(sel);
			}
		}
	}
	
	// Render minions
	for (ListNode *i = map.minions.first; i; i = i->next) {
		Minion *minion = i->el;
		gfxDrawSprite(0xf0, minion->pos);
	}
	
	// Render minimap
	{
		SDL_SetTextureBlendMode(gfx.minimap, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(gfx.minimap, 255 * 0.9);
		SDL_Rect dst = {
			TILE_SIZE, TILE_SIZE,
			map.size.x/gfx.minimapZoom, map.size.y/gfx.minimapZoom
		};
		SDL_RenderCopy(gfx.renderer, gfx.minimap, NULL, &dst);
	}
	
	SDL_RenderPresent(gfx.renderer);
}

void gfxMinimapRefresh() {
	Pixel *pixels;
	int pitch;
	
	int err = SDL_LockTexture(gfx.minimap, NULL, (void**) &pixels, &pitch);
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
	
	SDL_UnlockTexture(gfx.minimap);
}