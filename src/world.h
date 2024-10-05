#ifndef WORLD_H
#define WORLD_H

#include "entity.h"

#define WORLD_SIZE 256
#define TILE_SIZE 16

typedef struct{
	int tiles_bg[WORLD_SIZE][WORLD_SIZE];
	int tiles_fg[WORLD_SIZE][WORLD_SIZE];
} WorldProperties;

Entity * World_Create(Game *game);

void World_Load(Game *game, Entity *world, const char *filename);

int World_GetTileFG(Entity *world, int i, int j);

int World_GetTileBG(Entity *world, int i, int j);

int World_CanEntitySee(Entity *world, Entity *entity, Vec2 point);

int World_SolveCollisionForEntity(Entity *world, Entity *entity);

void World_Update(Game *game, Entity *world);

void World_Render(Game *game, Entity *world);

#endif
