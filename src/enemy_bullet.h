#ifndef ENEMY_BULLET_H
#define ENEMY_BULLET_H

#include "entity.h"

Entity * EnemyBullet_Create(Game *game);

Entity * EnemyBullet_CreateWithTrajectory(Game *game, Vec2 direction, Vec2 (*VelocityTrajectory)(double));

void EnemyBullet_SetDamage(Entity *entity, int damage);

void EnemyBullet_Update(Game *game, Entity *entity);

#endif
