#include "fixer.h"
#include "enemy_bullet.h"
#include "enemy_util.h"

#include <math.h>

#define FIXER_VELOCITY 5
#define FIXER_VISION 300
#define FIXER_VISION_SQR (FIXER_VISION * FIXER_VISION)
#define TIME_TO_SHOOT 1.6
#define SECOND_PER_FRAME 0.1
#define BULLET_VELOCITY 200
#define TIME_TO_CHANGE_VELOCITY 3

typedef struct{
	double timer_change_velocity;
	double timer_animation;
	double timer_shoot;
} FixerProperties;

static Vec2 Fixer_BulletTrajectory(double t);
static void Fixer_Shoot(Game *game, Entity *entity);
static void Fixer_Collider(Game *game, Entity *entity);

static Vec2 Fixer_BulletTrajectory(double t){
	return Vec2_Create(
			BULLET_VELOCITY,
			100 * cos(10 * t)
			);
}

static void Fixer_Shoot(Game *game, Entity *entity){
	FixerProperties *properties = (FixerProperties *) entity->properties;
	Vec2 dir, offset, position;
	Entity *bullet;
	Entity *player = Entity_Find(game, ENTITY_PLAYER);

	if(player == NULL) return;

	if(Entity_CheckCollision(player, entity))
		Entity_DoDamage(player, 10);

	properties->timer_shoot += game->delta_time;

	if(properties->timer_shoot < TIME_TO_SHOOT) return;

	properties->timer_shoot = 0;

	if(Vec2_LengthSqr(Vec2_Sub(player->position, entity->position)) > FIXER_VISION_SQR)
		return;

	for(int i = -1; i < 2; i++){
		for(int j = -1; j < 2; j++){
			if(i == 0 && j == 0) continue;

			/* calculate the offset */
			offset = Vec2_Create(i, j);
			offset = Vec2_Normalize(offset);
			dir = offset;
			offset = Vec2_Mul(offset, 30);
			offset = Vec2_Sub(offset, Vec2_Create(3, 3));

			position = Vec2_Add(
					Entity_GetHitboxCenter(entity),
					offset
					);


			bullet = EnemyBullet_CreateWithTrajectory(game, dir, Fixer_BulletTrajectory);

			bullet->position = position;

			bullet->velocity = Vec2_Mul(dir, BULLET_VELOCITY);

			Stack_Push(game->entity_stack, &bullet);
		}
	}
}

static void Fixer_Collider(Game *game, Entity *entity){
	Entity_RandomDirection(entity, FIXER_VELOCITY);
}

Entity * Fixer_Create(Game *game){
	Entity *entity = Entity_Create(game);

	entity->texture = game->texture_pack->fixer_texture;
	entity->texture_id = 0;
	entity->entity_type = ENTITY_FIXER;

	entity->hitbox_size = Vec2_Create(64, 64);
	entity->health = 343;

	entity->CustomUpdate = Fixer_Update;
	entity->CustomCollider = Fixer_Collider;
	entity->CustomDestructor = EnemyUtil_Destructor;

	entity->properties = malloc(sizeof(FixerProperties));

	FixerProperties *properties = (FixerProperties *) entity->properties;

	properties->timer_animation = 0;
	properties->timer_shoot = 0.4;
	properties->timer_change_velocity = 0.4;
	
	return entity;
}

void Fixer_Update(Game *game, Entity *entity){
	FixerProperties *properties = (FixerProperties *) entity->properties;
	int frame;

	properties->timer_animation += game->delta_time;

	frame = (int) (properties->timer_animation / SECOND_PER_FRAME) % 7;

	entity->texture_id = frame;

	Fixer_Shoot(game, entity);

	/*
	EnemyUtil_SetUpVelocity(
			game,
			entity,
			&properties->timer_change_velocity,
			TIME_TO_CHANGE_VELOCITY, 
			FIXER_VELOCITY
			); 
			*/
}
