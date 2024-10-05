#ifndef SHOOTER_H
#define SHOOTER_H

#include "entity.h"

Entity * Shooter_Create(Game *game);

void Shooter_Update(Game *game, Entity *entity);

#endif
