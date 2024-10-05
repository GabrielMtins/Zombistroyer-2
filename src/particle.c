#include "particle.h"
#include "draw.h"

#define INITIAL_VELOCITY 150
#define FRICTION -200

Entity * Particle_Create(Game *game){
	Entity *entity = Entity_Create(game);

	entity->entity_type = ENTITY_PARTICLE;
	entity->layer = LAYER_AIM;
	entity->is_rigid = 0;
	entity->hitbox_size = Vec2_Create(6, 6);
	entity->CustomUpdate = Particle_Update;
	entity->CustomRender = Particle_Render;

	Entity_RandomDirection(
			entity,
			INITIAL_VELOCITY / 2 + rand() % (INITIAL_VELOCITY / 2)
			);

	return entity;
}

void Particle_Update(Game *game, Entity *entity){
	Vec2 dir, acc;

	dir = Vec2_Normalize(entity->velocity);
	acc = Vec2_Mul(dir, game->delta_time * FRICTION);

	entity->velocity = Vec2_Add(entity->velocity, acc);

	if(Vec2_LengthSqr(entity->velocity) < 10) entity->is_dead = 1;
}

void Particle_Render(Game *game, Entity *entity){
	int rect[4] = {
		entity->position.x - game->camera.x,
		entity->position.y - game->camera.y,
		6,
		6
	};

	int color[4] = {
		255, 255, 255,
		255.0 * sqrt(Vec2_Length(entity->velocity) / INITIAL_VELOCITY)
	};

	Draw_DrawRect(&game->context, rect, color);
}
