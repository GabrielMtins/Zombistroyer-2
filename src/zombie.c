#include "zombie.h"
#include "world.h"
#include "enemy_util.h"

#define ZOMBIE_VELOCITY 40
#define ZOMBIE_VISION 200
#define ZOMBIE_DAMAGE 10 
#define ZOMBIE_VISION_SQR (ZOMBIE_VISION * ZOMBIE_VISION)
#define TIME_TO_CHANGE_VELOCITY 3

typedef struct{
	double timer_change_velocity;
} ZombieProperties;

static void Zombie_Collider(Game *game, Entity *entity);

static void Zombie_Collider(Game *game, Entity *entity){
	Entity_RandomDirection(entity, ZOMBIE_VELOCITY);
}

Entity * Zombie_Create(Game *game){
	Entity *entity = Entity_Create(game);

	entity->entity_type = ENTITY_ZOMBIE;
	entity->texture = game->texture_pack->zombie_texture;
	entity->hitbox_size = Vec2_Create(16, 32);
	entity->texture_id = 0;

	entity->CustomUpdate = Zombie_Update;
	entity->CustomCollider = Zombie_Collider;
	entity->CustomDestructor = EnemyUtil_Destructor;

	entity->properties = malloc(sizeof(ZombieProperties));

	((ZombieProperties *) entity->properties)->timer_change_velocity = 100;

	return entity;
}

void Zombie_Update(Game *game, Entity *entity){
	Entity *player = Entity_Find(game, ENTITY_PLAYER);
	ZombieProperties *properties = (ZombieProperties *) entity->properties;

	if(player == NULL) return;
	
	Vec2 diff = Vec2_Sub(player->position, entity->position);
	Vec2 dir;
	double dist_sqr = Vec2_LengthSqr(diff);

	if(dist_sqr < ZOMBIE_VISION_SQR && World_CanEntitySee(game->world, entity, player->position)){
		dir = Vec2_Normalize(diff);
		entity->velocity = Vec2_Mul(dir, 2 * ZOMBIE_VELOCITY);
	}
	else{
		EnemyUtil_SetUpVelocity(
				game,
				entity,
				&properties->timer_change_velocity,
				TIME_TO_CHANGE_VELOCITY,
				ZOMBIE_VELOCITY
				);
	}

	if(entity->velocity.x > 0)
		entity->texture_id = 0;
	if(entity->velocity.x < 0)
		entity->texture_id = 1;

	if(Entity_CheckCollision(entity, player)){
		Entity_DoDamage(player, ZOMBIE_DAMAGE);
	}

	Entity_SolveCollisionFor(game, entity, util_enemy_list);
}
