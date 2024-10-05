#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "vec2.h"
#include "stack.h"
#include "context.h"
#include "texture.h"
#include "sfx.h"

typedef struct{
	Texture *player_texture;
	Texture *world_texture;
	Texture *aim_texture;
	Texture *shadow_texture;
	Texture *player_shadow_texture;
	Texture *bullet_texture;
	Texture *enemybullet_texture;
	Texture *zombie_texture;
	Texture *shooter_texture;
	Texture *fixer_texture;
	Texture *stringer_texture;
	Texture *shotgun_texture;
	Texture *pickup_texture;
	Texture *door_texture;
	Texture *door_shadow_texture;
	Texture *particle_texture;
	Texture *uibutton_texture;

	Sfx *mainmenu_music;
	Sfx *level_theme[5];
	Sfx *collectkey_sfx;
	Sfx *bullet_sfx;
	Sfx *dooropen_sfx;
	Sfx *enemydamage_sfx;
	Sfx *dialog_sfx;
} TexturePack;

enum Layers{
	LAYER_BACKGROUND = 0,
	LAYER_SHADOW,
	LAYER_FOREGROUND,
	LAYER_PICKUP,
	LAYER_SPRITE,
	LAYER_PLAYER,
	LAYER_AIM,
	LAYER_HUD,
	LAYER_DIALOG,
};

enum EntityType{
	ENTITY_NONE = -1,
	ENTITY_PLAYER,
	ENTITY_AIM,
	ENTITY_BULLET,
	ENTITY_ENEMYBULLET,
	ENTITY_ZOMBIE,
	ENTITY_SHOOTER,
	ENTITY_FIXER,
	ENTITY_STRINGER,
	ENTITY_SHOTGUN,
	ENTITY_PICKUP,
	ENTITY_DOOR,
	ENTITY_PARTICLE,
	ENTITY_HUD,
	ENTITY_UIBUTTON,
	ENTITY_DIALOGBOX,
};

TexturePack * TexturePack_Load(Context *context);
void TexturePack_Destroy(TexturePack *texture_pack);

typedef struct{
	TexturePack *texture_pack;
	Context context;
	void *world;
	void *hud;
	Stack *entity_stack;

	int internal_width, internal_height;
	int inverted_fps;
	int current_layer;

	Vec2 camera;

	int quit;
	double delta_time;
} Game;

Game * Game_Create(void);

void Game_SetDrawLayer(Game *game, int i);

void Game_SetOpacityLayer(Game *game, int i, uint8_t a);

void Game_CleanEntityStack(Game *game);

void Game_Run(Game *game);

void Game_Destroy(Game *game);

#endif
