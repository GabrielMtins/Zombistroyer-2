#ifndef ZOMBIE_H
#define ZOMBIE_H

#include "entity.h"

Entity * Zombie_Create(Game *game);

void Zombie_Update(Game *game, Entity *entity);

#endif
