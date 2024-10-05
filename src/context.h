#ifndef CONTEXT_H
#define CONTEXT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>

#define NUMBER_LAYERS 10
#define PI 3.141592

typedef struct{
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *layer[NUMBER_LAYERS];
	SDL_GameController *gamepad;
	TTF_Font *font;
} Context;

#endif
