#ifndef TEXTURE_H
#define TEXTURE_H

#include "context.h"

typedef struct{
	SDL_Texture *texture;
	SDL_Texture *texture_light;
	int cell_width, cell_height;
	int texture_width, texture_height;
} Texture;

Texture * Texture_Load(Context *context, const char *filename, int cell_width, int cell_height);

void Texture_RenderCell(Context *context, Texture *texture, int x, int y, int id);

void Texture_RenderCellLighted(Context *context, Texture *texture, int x, int y, int id);

void Texture_Destroy(Texture *texture);

#endif
