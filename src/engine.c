#include "bored.h"

struct Engine engine;

void engineCheckEvent(SDL_Event *event) {
	switch (event->type) {
	case SDL_QUIT:
		engine.running = false;
		break;
	
	case SDL_MOUSEMOTION:
		if (event->motion.state == SDL_BUTTON_RMASK) {
			gfx.offset.x += event->motion.xrel;
			gfx.offset.y += event->motion.yrel;
		}
		map.hover = (Vector){
			(event->motion.x - gfx.offset.x) / (TILE_SIZE*gfx.zoom),
			(event->motion.y - gfx.offset.y) / (TILE_SIZE*gfx.zoom),
		};
		break;
	
	case SDL_MOUSEBUTTONDOWN:
		if (event->button.button == SDL_BUTTON_LEFT) {
			mapAddTask(map.hover);
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
	mapSeed(rand());
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