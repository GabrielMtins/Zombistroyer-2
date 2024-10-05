#ifndef DOOR_H
#define DOOR_H

#include "entity.h"

Entity * Door_Create(Game *game, int color);

void Door_Update(Game *game, Entity *entity);

void Door_Render(Game *game, Entity *entity);

#endif
