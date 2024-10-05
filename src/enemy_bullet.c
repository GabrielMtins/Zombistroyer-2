#include "enemy_bullet.h"

#define TIME_TO_LIVE 10

typedef struct{
	double time_alive;
	Vec2 direction;
	int damage;
	Vec2 (*VelocityTrajectory)(double);
} EnemyBulletProperties;

static void EnemyBullet_Collider(Game *game, Entity *entity);
static void EnemyBullet_UpdateVelocity(Entity *entity);

static void EnemyBullet_Collider(Game *game, Entity *entity){
	entity->is_dead = 1;
}

static void EnemyBullet_UpdateVelocity(Entity *entity){
	EnemyBulletProperties *properties = (EnemyBulletProperties *) entity->properties;
	Vec2 dir_x, dir_y;

	if(properties->VelocityTrajectory == NULL) return;
	
	dir_x = properties->direction;
	dir_y = Vec2_Create(properties->direction.y, -properties->direction.x);

	Vec2 velocity_abs = properties->VelocityTrajectory(properties->time_alive);

	entity->velocity = 
		Vec2_Add(
				Vec2_Mul(dir_x, velocity_abs.x),
				Vec2_Mul(dir_y, velocity_abs.y)
				);
}

Entity * EnemyBullet_Create(Game *game){
	Entity *entity = Entity_Create(game);

	entity->texture = game->texture_pack->enemybullet_texture;
	entity->hitbox_size = Vec2_Create(7, 7);
	entity->entity_type = ENTITY_ENEMYBULLET;

	entity->CustomUpdate = EnemyBullet_Update;
	entity->CustomCollider = EnemyBullet_Collider;

	entity->properties = malloc(sizeof(EnemyBulletProperties));

	EnemyBulletProperties *properties = (EnemyBulletProperties *) entity->properties;

	properties->time_alive = 0;
	properties->damage = 10;
	properties->VelocityTrajectory = NULL;
	properties->direction = Vec2_Create(0, 0);

	return entity;
}

Entity * EnemyBullet_CreateWithTrajectory(Game *game, Vec2 direction, Vec2 (*VelocityTrajectory)(double)){
	Entity *entity = EnemyBullet_Create(game);

	EnemyBulletProperties *properties = (EnemyBulletProperties *) entity->properties;

	properties->VelocityTrajectory = VelocityTrajectory;
	properties->direction = direction;

	return entity;
}

void EnemyBullet_SetDamage(Entity *entity, int damage){
	EnemyBulletProperties *properties = (EnemyBulletProperties *) entity->properties;

	properties->damage = damage;
}

void EnemyBullet_Update(Game *game, Entity *entity){
	Entity *player = Entity_Find(game, ENTITY_PLAYER);
	EnemyBulletProperties *properties = (EnemyBulletProperties *) entity->properties;
	Entity **tmp;

	properties->time_alive += game->delta_time;

	/* Update in case it has a custom trajectory */
	EnemyBullet_UpdateVelocity(entity);

	if(properties->time_alive > TIME_TO_LIVE) entity->is_dead = 1;

	if(player == NULL){
		entity->is_dead = 1;
	}
	else{
		if(Entity_CheckCollision(entity, player)){
			entity->is_dead = 1;
			Entity_DoDamage(player, properties->damage);
		}
	}

	for(size_t i = 0; i < game->entity_stack->size; i++){
		tmp = Stack_Get(game->entity_stack, i);

		if((*tmp)->entity_type == ENTITY_DOOR && Entity_CheckCollision(*tmp, entity)){
			entity->is_dead = 1; /* kills itself if it finds a enemy */
			break;
		}
	}
}
