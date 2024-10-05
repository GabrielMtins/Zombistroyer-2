#include "bullet.h"
#include "particle.h"

#define TIME_TO_DIE 4

static const int enemy_list[] = {
	ENTITY_ZOMBIE,
	ENTITY_SHOOTER,
	ENTITY_FIXER,
	ENTITY_STRINGER,
	ENTITY_SHOTGUN,
	ENTITY_DOOR,
	ENTITY_NONE // mark the end of the list
};

typedef struct{
	double time_alive;
	int damage;
} BulletProperties;

static void Bullet_MakeParticles(Game *game, Entity *enemy);
static void Bullet_Collider(Game *game, Entity *entity);
static void Bullet_FindEnemies(Game *game, Entity *entity);

static void Bullet_MakeParticles(Game *game, Entity *enemy){
	/*
	Entity *particle = Particle_Create(game, damage);
	particle->position = Entity_GetHitboxCenter(enemy);
	Stack_Push(game->entity_stack, &particle);
	*/
}

static void Bullet_Collider(Game *game, Entity *entity){
	entity->is_dead = 1;
}

static void Bullet_FindEnemies(Game *game, Entity *entity){
	Entity **tmp;
	BulletProperties *properties = (BulletProperties *) entity->properties;
	int done_damage = 0;

	for(size_t i = 0; i < game->entity_stack->size; i++){
		tmp = Stack_Get(game->entity_stack, i);

		if(entity->is_dead) break;

		for(int j = 0; enemy_list[j] != ENTITY_NONE; j++){
			if((*tmp)->entity_type == enemy_list[j] && Entity_CheckCollision(*tmp, entity)){
				if(enemy_list[j] != ENTITY_DOOR) 
					done_damage = Entity_DoDamage(*tmp, properties->damage);

				if(done_damage)
					Sfx_Play(game->texture_pack->enemydamage_sfx, 0, 1);

				entity->is_dead = 1; /* kills itself if it finds a enemy */
				break;
			}
		}
	}
}

Entity * Bullet_Create(Game *game){
	Entity *entity = Entity_Create(game);

	entity->hitbox_size = Vec2_Create(5, 5);
	entity->texture = game->texture_pack->bullet_texture;
	entity->texture_id = -1;
	entity->entity_type = ENTITY_BULLET;
	entity->CustomUpdate = Bullet_Update;
	entity->CustomCollider = Bullet_Collider;

	entity->properties = malloc(sizeof(BulletProperties));

	BulletProperties *properties = (BulletProperties *) entity->properties;

	properties->time_alive = 0;
	properties->damage = 10;

	Sfx_Play(game->texture_pack->bullet_sfx, 0, 0);

	return entity;
}

void Bullet_SetDamage(Entity *entity, int damage){
	BulletProperties *properties = (BulletProperties *) entity->properties;
	properties->damage = damage;
}

void Bullet_Update(Game *game, Entity *entity){
	BulletProperties *properties = (BulletProperties *) entity->properties;

	properties->time_alive += game->delta_time;

	if(properties->time_alive > TIME_TO_DIE) entity->is_dead = 1;

	Bullet_FindEnemies(game, entity);
}
