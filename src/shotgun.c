#include "shotgun.h"
#include "world.h"
#include "enemy_bullet.h"
#include "enemy_util.h"

#define MIN_DISTANCE_SQR (120 * 120)
#define SHOTGUN_VISION (240)
#define SHOTGUN_VISION_SQR (SHOTGUN_VISION * SHOTGUN_VISION)
#define SHOTGUN_VELOCITY 60
#define SHOTGUN_CALM_VELOCITY 30
#define TIME_TO_CHANGE_VELOCITY 3
#define TIME_TO_SHOOT 1.6
#define BULLET_VELOCITY 200
#define SECOND_PER_FRAME 0.2

#define COS_ANGLE 0.8660
#define SIN_ANGLE 0.5

/* This enemy goes to the player with a sin wave pattern */

typedef struct{
	double timer_change_velocity;
	double timer_velocity_direction;
	double timer_shoot;
	double timer_animation;
} ShotgunProperties;

static void Shotgun_AddBullet(Game *game, Entity *entity, Vec2 dir);
static void Shotgun_Fire(Game *game, Entity *entity, Entity *player);
static void Shotgun_Collider(Game *game, Entity *entity);
static Vec2 Shotgun_CalculateVelocity(Entity *entity, Vec2 direction);

static void Shotgun_AddBullet(Game *game, Entity *entity, Vec2 dir){
	Entity *enemybullet = EnemyBullet_Create(game); 

	// if it's facing left
	if(entity->velocity.x < 0)
		enemybullet->position = Vec2_Add(entity->position, Vec2_Create(0, 2));
	else // or if it's facing right
		enemybullet->position = Vec2_Add(entity->position, Vec2_Create(26, 2));

	enemybullet->velocity = Vec2_Mul(dir, BULLET_VELOCITY);

	Stack_Push(game->entity_stack, &enemybullet);
}

static void Shotgun_Fire(Game *game, Entity *entity, Entity *player){
	ShotgunProperties *properties = (ShotgunProperties *) entity->properties;
	Vec2 dir, dir_ortho, new_dir;

	if(properties->timer_shoot < TIME_TO_SHOOT) return;

	properties->timer_shoot = 0;

	dir = Vec2_Sub(
			Entity_GetHitboxCenter(player),
			Vec2_Add(entity->position, Vec2_Create(3, 3))
			);

	dir = Vec2_Normalize(dir);
	dir_ortho = Vec2_Create(dir.y, -dir.x);
	
	Shotgun_AddBullet(game, entity, dir);

	new_dir = Vec2_Add(
			Vec2_Mul(dir, COS_ANGLE),
			Vec2_Mul(dir_ortho, SIN_ANGLE)
			);

	Shotgun_AddBullet(game, entity, new_dir);

	new_dir = Vec2_Add(
			Vec2_Mul(dir, COS_ANGLE),
			Vec2_Mul(dir_ortho, -SIN_ANGLE)
			);

	Shotgun_AddBullet(game, entity, new_dir);
}

static void Shotgun_Collider(Game *game, Entity *entity){
	Entity_RandomDirection(entity, SHOTGUN_CALM_VELOCITY);
}

static Vec2 Shotgun_CalculateVelocity(Entity *entity, Vec2 direction){
	ShotgunProperties *properties = (ShotgunProperties *) entity->properties;
	Vec2 direction_ortho;
	Vec2 real_velocity;
	Vec2 transformed_velocity;

	direction_ortho = Vec2_Create(direction.y, -direction.x);
	
	real_velocity = Vec2_Create(
				SHOTGUN_VELOCITY,
				2 * SHOTGUN_VELOCITY * cos(8 * properties->timer_velocity_direction)
			);

	transformed_velocity = Vec2_Add(
			Vec2_Mul(direction, real_velocity.x),
			Vec2_Mul(direction_ortho, real_velocity.y)
			);

	return transformed_velocity;
}

Entity * Shotgun_Create(Game *game){
	Entity *entity = Entity_Create(game);

	entity->texture = game->texture_pack->shotgun_texture;

	entity->hitbox_size = Vec2_Create(16, 30);
	entity->hitbox_offset = Vec2_Create(8, 2);
	entity->texture_id = 0;
	entity->entity_type = ENTITY_SHOTGUN;

	entity->CustomUpdate = Shotgun_Update;
	entity->CustomCollider = Shotgun_Collider;
	entity->CustomDestructor = EnemyUtil_Destructor;

	entity->properties = malloc(sizeof(ShotgunProperties));

	ShotgunProperties *properties = (ShotgunProperties *) entity->properties;

	properties->timer_change_velocity = 0;
	properties->timer_velocity_direction = 0;
	properties->timer_animation = 0;
	
	return entity;
}

void Shotgun_Update(Game *game, Entity *entity){
	Entity *player = Entity_Find(game, ENTITY_PLAYER);
	ShotgunProperties *properties = (ShotgunProperties *) entity->properties;
	double distance_to_player;
	int frame = 0;
	Vec2 dir;
	Vec2 dir_ortho;

	if(player == NULL) return;

	properties->timer_velocity_direction += game->delta_time;
	properties->timer_shoot += game->delta_time;
	properties->timer_animation += game->delta_time;

	distance_to_player = Entity_DistSqrBetween(entity, player);
	dir = Vec2_Normalize(Vec2_Sub(
				Entity_GetHitboxCenter(player),
				Entity_GetHitboxCenter(entity)
				));

	dir_ortho = Vec2_Create(dir.y, -dir.x);

	if(distance_to_player < SHOTGUN_VISION_SQR && World_CanEntitySee(game->world, entity, player->position)){
		if(distance_to_player > MIN_DISTANCE_SQR){
			entity->velocity = Shotgun_CalculateVelocity(entity, dir);
		}
		else{
			//entity->velocity = Shotgun_CalculateVelocity(entity, dir_ortho);
			//entity->velocity = Vec2_Mul(dir_ortho, SHOTGUN_VELOCITY);
			entity->velocity = Vec2_Create(0, 0);
		}

		Shotgun_Fire(game, entity, player);
	}
	else{
		EnemyUtil_SetUpVelocity(
				game,
				entity,
				&properties->timer_change_velocity,
				TIME_TO_CHANGE_VELOCITY,
				SHOTGUN_CALM_VELOCITY
				);
	}

	if(entity->velocity.x > 0) entity->texture_id = 0;
	else entity->texture_id = 3;

	if(Vec2_LengthSqr(entity->velocity) > 10){
		frame = (int) (properties->timer_animation / SECOND_PER_FRAME) % 2;
		entity->texture_id += frame;
	}
	else{
		if(entity->position.x < player->position.x) entity->texture_id = 0;
		else entity->texture_id = 3;
	}
}
