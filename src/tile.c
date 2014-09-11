#include "bored.h"


struct TileType tileTypes[256] = {
	[0x00] = {1}, // dirt
	[0x01] = {0, {4, 0x04}}, // rock
	[0x02] = {0}, // water
	[0x03] = {1}, // sand
	[0x04] = {1}, // gravel
	
	[0x10] = {0, {16, 0x00}}, // tree
};