#ifndef PICKUP_H
#define PICKUP_H

#include "entity.h"

enum KeyColors{
	KEY_RED = 0,
	KEY_BLUE,
	KEY_GREEN,
	KEY_GOLD,
	KEY_NUM
};

Entity * Pickup_Create(Game *game);

Entity * Pickup_CreateKey(Game *game, int color);

Entity * Pickup_CreateHealthKit(Game *game);

Entity * Pickup_CreateBulletKit(Game *game, int type);

void Pickup_Update(Game *game, Entity *entity);

#endif
