#include "enemy_util.h"
#include "particle.h"
#include "hud.h"

const int util_enemy_list[] = {
	ENTITY_ZOMBIE,
	ENTITY_SHOOTER,
	ENTITY_FIXER,
	ENTITY_STRINGER,
	ENTITY_SHOTGUN,
	ENTITY_NONE // mark the end of the list
};

void EnemyUtil_SetUpVelocity(Game *game, Entity *entity, double *timer, double time_to_change_velocity, double velocity){
	int random_number = 0;

	*timer += game->delta_time;

	if(*timer < time_to_change_velocity) return;

	random_number = rand() % 100;

	if(random_number > 20){
		if(random_number > 60)
			Entity_RandomDirection(entity, velocity);
		else
			entity->velocity = Vec2_Create(0, 0);
	}

	*timer = 0;
}

void EnemyUtil_Destructor(Game *game, Entity *entity){
	if(!entity->is_dead) return;

	Entity *particle;

	for(int i = 0; i < 30; i++){
		particle = Particle_Create(game);
		particle->position = Entity_GetHitboxCenter(entity);

		Stack_Push(game->entity_stack, &particle);
	}

	Hud_AddToEnemyCounter(game->hud);
}
