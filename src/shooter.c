#include "shooter.h"
#include "world.h"
#include "enemy_bullet.h"
#include "enemy_util.h"

#define MIN_DISTANCE_SQR (60 * 60)
#define BULLET_VELOCITY 250
#define BULLET_DAMAGE 20
#define SHOOTER_VELOCITY 40
#define SHOOTER_VISION 200
#define SHOOTER_VISION_SQR (SHOOTER_VISION * SHOOTER_VISION)
#define TIME_TO_CHANGE_VELOCITY 3
#define TIME_TO_SHOOT 0.8
#define SECOND_PER_FRAME 0.2

typedef struct{
	double timer_animation;
	double timer_shoot;
	double timer_change_velocity;
} ShooterProperties;

static void Shooter_Collider(Game *game, Entity *entity);
static void Shooter_Fire(Game *game, Entity *entity, Entity *player);

static void Shooter_Collider(Game *game, Entity *entity){
	Entity_RandomDirection(entity, SHOOTER_VELOCITY);
}

static void Shooter_Fire(Game *game, Entity *entity, Entity *player){
	ShooterProperties *properties = (ShooterProperties *) entity->properties;
	Vec2 dir;
	Entity *enemybullet;

	if(properties->timer_shoot < TIME_TO_SHOOT) return;

	properties->timer_shoot = 0;

	enemybullet = EnemyBullet_Create(game); 
	enemybullet->position = entity->position;

	// if it's facing left
	if(entity->texture_id > 1)
		enemybullet->position = Vec2_Add(entity->position, Vec2_Create(0, 2));
	else // or if it's facing right
		enemybullet->position = Vec2_Add(entity->position, Vec2_Create(26, 2));

	dir = Vec2_Sub(
			Entity_GetHitboxCenter(player),
			Entity_GetHitboxCenter(enemybullet)
			);

	dir = Vec2_Normalize(dir);

	enemybullet->velocity = Vec2_Mul(dir, BULLET_VELOCITY);
	EnemyBullet_SetDamage(enemybullet, BULLET_DAMAGE);

	Stack_Push(game->entity_stack, &enemybullet);
}

Entity * Shooter_Create(Game *game){
	Entity *entity = Entity_Create(game);

	entity->texture = game->texture_pack->shooter_texture;
	entity->hitbox_size = Vec2_Create(27, 32);
	entity->hitbox_offset = Vec2_Create(3, 0);
	entity->health = 170;

	entity->texture_id = 0;
	entity->entity_type = ENTITY_SHOOTER;

	entity->properties = malloc(sizeof(ShooterProperties));

	entity->CustomUpdate = Shooter_Update;
	entity->CustomCollider = Shooter_Collider;
	entity->CustomDestructor = EnemyUtil_Destructor;

	return entity;
}

void Shooter_Update(Game *game, Entity *entity){
	Entity *player = Entity_Find(game, ENTITY_PLAYER);
	ShooterProperties *properties = (ShooterProperties *) entity->properties;

	Vec2 diff, dir;
	double dist_sqr;
	int frame = 0;

	if(player == NULL) return;

	properties->timer_animation += game->delta_time;
	properties->timer_shoot += game->delta_time;
	
	diff = Vec2_Sub(Entity_GetHitboxCenter(player), entity->position);
	dist_sqr = Vec2_LengthSqr(diff);
	dir = Vec2_Normalize(diff);

	if(dist_sqr < SHOOTER_VISION_SQR && World_CanEntitySee(game->world, entity, Entity_GetHitboxCenter(player))){
		entity->velocity = Vec2_Create(0, 0);

		if(dist_sqr > MIN_DISTANCE_SQR){
			entity->velocity = Vec2_Mul(dir, SHOOTER_VELOCITY);
		}

		Shooter_Fire(game, entity, player);
	}
	else{
		EnemyUtil_SetUpVelocity(
				game,
				entity,
				&properties->timer_change_velocity,
				TIME_TO_CHANGE_VELOCITY,
				SHOOTER_VELOCITY
				);
	}

	if(entity->velocity.x > 0)
		entity->texture_id = 0;
	if(entity->velocity.x < 0)
		entity->texture_id = 2;

	if(Vec2_LengthSqr(entity->velocity) > 10){
		frame = (int) (properties->timer_animation / SECOND_PER_FRAME) % 2;
		entity->texture_id += frame;
	}

	if(Entity_CheckCollision(entity, player)){
		Entity_DoDamage(player, 10);
	}

	Entity_SolveCollisionFor(game, entity, util_enemy_list);
}
