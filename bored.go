package main

import (
	"math/rand"
	"unsafe"
)

// #include <stdlib.h>
// #include <SDL2/SDL.h>
// #include <SDL2/SDL_image.h>
// #cgo pkg-config: sdl2 SDL2_image
import "C"


// Constants

const TILE_SIZE = 16;

// Globals

var engine struct {
	window *C.SDL_Window
	running bool
	frame int
}

var gfx struct {
	renderer *C.SDL_Renderer
	tileTexture *C.SDL_Texture
	offsetX, offsetY int
	zoom int
}

var grid struct {
	w, h int
	tiles []Tile
	hoverX, hoverY int
	playerX, playerY int
	playerDestX, playerDestY int
}

type Tile uint8


// Grid

func GridInit() {
	grid.w = 64
	grid.h = 64
	grid.tiles = make([]Tile, grid.w * grid.h)
	for y := 0; y < grid.h; y++ {
		for x := 0; x < grid.w; x++ {
			*GridGetTile(x, y) = Tile(rand.Intn(2))
		}
	}
	grid.hoverX = -1
	grid.hoverY = -1
	grid.playerX = 0
	grid.playerY = 0
}

func GridGetTile(x, y int) *Tile {
	if 0 <= x && x < grid.w && 0 <= y && y < grid.h {
		return &grid.tiles[x + grid.w * y]
	} else {
		return nil
	}
}

// Graphics

func GfxInit() {
	gfx.renderer = C.SDL_CreateRenderer(engine.window, -1, 0x00000002 | 0x00000004)
	
	tileFilename := C.CString("tiles.png")
	tileSurface := C.IMG_Load(tileFilename)
	C.free(unsafe.Pointer(tileFilename))
	gfx.tileTexture = C.SDL_CreateTextureFromSurface(gfx.renderer, tileSurface)
	C.SDL_FreeSurface(tileSurface)
	
	gfx.zoom = 2
}

func GfxDestroy() {
	C.SDL_DestroyTexture(gfx.tileTexture);
	C.SDL_DestroyRenderer(gfx.renderer);
}

func GfxDrawTile(tile Tile, x, y int) {
	src := C.SDL_Rect {
		C.int(tile * TILE_SIZE), 0,
		TILE_SIZE, TILE_SIZE,
	}
	dst := C.SDL_Rect {
		C.int(gfx.offsetX + x*TILE_SIZE*gfx.zoom),
		C.int(gfx.offsetY + y*TILE_SIZE*gfx.zoom),
		C.int(TILE_SIZE*gfx.zoom), C.int(TILE_SIZE*gfx.zoom),
	}
	C.SDL_RenderCopy(gfx.renderer, gfx.tileTexture, &src, &dst)
}

func GfxRender() {
	C.SDL_RenderClear(gfx.renderer)
	
	// Render grid
	for y := 0; y < grid.h; y++ {
		for x := 0; x < grid.w; x++ {
			tile := *GridGetTile(x, y)
			hover := (x == grid.hoverX && y == grid.hoverY)
			
			if hover {
				C.SDL_SetTextureColorMod(gfx.tileTexture, 255, 128, 128)
			}
			GfxDrawTile(tile, x, y)
			if hover {
				C.SDL_SetTextureColorMod(gfx.tileTexture, 255, 255, 255)
			}
		}
	}
	
	// Render player
	GfxDrawTile(2, grid.playerX, grid.playerY)
	
	C.SDL_RenderPresent(gfx.renderer)
}

// Physics

func PhysStep() {
	// TODO
	/*
	The physics timestep currently walks the player toward the player destination,
	ignoring obstacles.
	It should run a breadth-first search on the map from the player location to
	the player destination, and step once in the correct direction on every frame.
	
	The map obstacles can be detected by
	tile := GridGetTile(x, y)
	if tile == nil || *tile != 0 {
		// Collision!
	}
	*/
	
	if grid.playerX < grid.playerDestX {
		grid.playerX++
	} else if grid.playerX > grid.playerDestX {
		grid.playerX--
	}
	if grid.playerY < grid.playerDestY {
		grid.playerY++
	} else if grid.playerY > grid.playerDestY {
		grid.playerY--
	}
}

// Engine

func EngineCheckEvent(event *C.SDL_Event) {
	eventPointer := unsafe.Pointer(event)
	eventType := (*uint32)(eventPointer)
	
	switch *eventType {
	case 0x100: // SDL_QUIT
		engine.running = false
	
	case 0x400: // SDL_MOUSEMOTION
		e := (*C.SDL_MouseMotionEvent)(eventPointer)
		if e.state == (1<<(3-1)) {
			gfx.offsetX += int(e.xrel)
			gfx.offsetY += int(e.yrel)
		}
		grid.hoverX = (int(e.x) - gfx.offsetX) / (TILE_SIZE*gfx.zoom)
		grid.hoverY = (int(e.y) - gfx.offsetY) / (TILE_SIZE*gfx.zoom)
	
	case 0x401: // SDL_MOUSEBUTTONDOWN
		e := (*C.SDL_MouseButtonEvent)(eventPointer)
		if e.button == 1 {
			grid.playerDestX = grid.hoverX
			grid.playerDestY = grid.hoverY
		}
	}
}

func EngineRun() {
	engine.running = true
	engine.frame = 0
	
	for engine.running {
		var event C.SDL_Event
		for C.SDL_PollEvent(&event) != 0 {
			EngineCheckEvent(&event)
		}
		
		if engine.frame % 8 == 0 {
			PhysStep()
		}
		
		GfxRender()
		engine.frame++
	}
}

func EngineInit() {
	err := C.SDL_Init(0x00000020)
	if err != 0 {
		panic("Could not initialize SDL2")
	}
	
	engine.window = C.SDL_CreateWindow(nil, 0x1FFF0000, 0x1FFF0000, 1024, 768, 0)
	if engine.window == nil {
		panic("Could not open window")
	}
	
	GfxInit()
	GridInit()
}

func EngineDestroy() {
	GfxDestroy()
	C.SDL_DestroyWindow(engine.window)
	C.SDL_Quit()
}

func main() {
	EngineInit()
	EngineRun()
	EngineDestroy()
}