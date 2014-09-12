#include "bored.h"

struct Engine engine;


void engineRun() {
	engine.running = true;
	engine.frame = 0;
	while (engine.running) {
		// Check events since the last frame
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			// Handle event
			if (event.type == SDL_QUIT) {
				engine.running = false;
			}
			else if (guiHandleEvent(&event)) {}
			else if (mapHandleEvent(&event)) {}
		}
		
		if (engine.frame % 8 == 0) {
			physStep();
		}
		
		gfxRender();
		
		engine.frame++;
	}
}

void engineInit() {
	int err;
	err = SDL_Init(SDL_INIT_VIDEO);
	assert(!err);
	
	err = TTF_Init();
	assert(!err);
	
	engine.window = SDL_CreateWindow("bored", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, 0);
	assert(engine.window);
	
	mapInit();
	mapSeed(rand());
	gfxInit();
	guiInit();
}

void engineDestroy() {
	mapDestroy();
	guiDestroy();
	gfxDestroy();
	SDL_DestroyWindow(engine.window);
	TTF_Quit();
	SDL_Quit();
}


int main() {
	engineInit();
	engineRun();
	engineDestroy();
	return 0;
} 