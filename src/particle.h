#ifndef PARTICLE_H
#define PARTICLE_H

#include "entity.h"

enum ParticleType{
	PARTICLE_ENEMY = 0,
};

Entity * Particle_Create(Game *game);

void Particle_Update(Game *game, Entity *entity);

void Particle_Render(Game *game, Entity *entity);

#endif
