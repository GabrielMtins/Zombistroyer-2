#include "sfx.h"

Sfx * Sfx_LoadSfx(const char *file){
	Sfx *sfx = (Sfx *) malloc(sizeof(Sfx));

	sfx->type = SFX_SFX;
	sfx->data.chunk = Mix_LoadWAV(file);

	if(sfx->data.chunk == NULL)
		fprintf(stderr, "Failed to load sound effect: %s\n", file);

	return sfx;
}

Sfx * Sfx_LoadMusic(const char *file){
	Sfx *sfx = (Sfx *) malloc(sizeof(Sfx));

	sfx->type = SFX_MUSIC;
	sfx->data.music = Mix_LoadMUS(file);

	if(sfx->data.music == NULL)
		fprintf(stderr, "Failed to load music: %s\n", file);

	return sfx;
}

void Sfx_SetVolume(Sfx *sfx, int volume){
	if(sfx == NULL) return;

	if(sfx->type == SFX_MUSIC){
		Mix_VolumeMusic(volume);
	}
	else{
		Mix_VolumeChunk(sfx->data.chunk, volume);
	}
}

void Sfx_FadeOutMusic(int ms){
	Mix_FadeOutMusic(ms);
}

void Sfx_Play(Sfx *sfx, int loops, int channel){
	if(sfx == NULL) return;

	if(sfx->type == SFX_MUSIC){
		Mix_PlayMusic(sfx->data.music, loops);
	}
	else{
		Mix_PlayChannel(channel, sfx->data.chunk, loops);
	}
}

void Sfx_Destroy(Sfx *sfx){
	if(sfx == NULL)	return;

	if(sfx->type == SFX_MUSIC){
		if(sfx->data.music != NULL) Mix_FreeMusic(sfx->data.music);
	}
	else{
		if(sfx->data.chunk != NULL) Mix_FreeChunk(sfx->data.chunk);
	}

	free(sfx);
}
