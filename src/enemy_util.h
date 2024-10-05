#ifndef ENEMY_UTIL_H
#define ENEMY_UTIL_H

#include "entity.h"

extern const int util_enemy_list[];

/* it updates the timer automatically */
void EnemyUtil_SetUpVelocity(Game *game, Entity *entity, double *timer, double time_to_change_velocity, double velocity);

void EnemyUtil_Destructor(Game *game, Entity *entity);

#endif
