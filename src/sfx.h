#ifndef SFX_H
#define SFX_H

#include "context.h"

enum SfxType{
	SFX_MUSIC = 0,
	SFX_SFX,
};

typedef struct{
	union {
		Mix_Chunk *chunk;
		Mix_Music *music;
	} data;

	int type;
} Sfx;

Sfx * Sfx_LoadSfx(const char *file);
Sfx * Sfx_LoadMusic(const char *file);

void Sfx_SetVolume(Sfx *sfx, int volume);

void Sfx_FadeOutMusic(int ms);

void Sfx_Play(Sfx *sfx, int loops, int channel);

void Sfx_Destroy(Sfx *sfx);

#endif
