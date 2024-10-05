#include "stringer.h"
#include "enemy_util.h"
#include "world.h"

#define TIME_JUMP ((double) STRINGER_VISION / STRINGER_JUMP_VELOCITY * 0.8)
#define TIME_TRY 1.5
#define DAMAGE_WHEN_JUMPING 40
#define DAMAGE_NORMAL 10
#define SECONDS_PER_FRAME 0.4
#define STRINGER_BASE_VELOCITY 15
#define STRINGER_JUMP_VELOCITY 250
#define STRINGER_VISION 300
#define STRINGER_VISION_SQR (STRINGER_VISION * STRINGER_VISION)

typedef struct{
	int is_jumping;
	double timer_try;
	double timer_jump;
	double timer_animation;
} StringerProperties;

static void Stringer_Collider(Game *game, Entity *entity);
static void Stringer_Jump(Game *game, Entity *entity, Entity *player);
static void Stringer_ResetJump(Entity *entity);

void Stringer_Collider(Game *game, Entity *entity){
	StringerProperties *properties = (StringerProperties *) entity->properties;

	Entity_RandomDirection(entity, STRINGER_BASE_VELOCITY);

	properties->is_jumping = 0;
}

static void Stringer_Jump(Game *game, Entity *entity, Entity *player){
	StringerProperties *properties = (StringerProperties *) entity->properties;
	Vec2 dir;

	if(player == NULL) return;

	if(properties->is_jumping == 1) return;
	if(Entity_DistSqrBetween(player, entity) > STRINGER_VISION_SQR) return;
	if(!World_CanEntitySee(game->world, entity, Entity_GetHitboxCenter(player))) return;

	properties->timer_jump = 0;
	properties->is_jumping = 1;

	dir = Vec2_Normalize(Vec2_Sub(
				Entity_GetHitboxCenter(player),
				Entity_GetHitboxCenter(entity)
				));

	entity->velocity = Vec2_Mul(dir, STRINGER_JUMP_VELOCITY);
}

static void Stringer_ResetJump(Entity *entity){
	StringerProperties *properties = (StringerProperties *) entity->properties;

	if(properties->timer_jump > TIME_JUMP){
		properties->is_jumping = 0;
		properties->timer_jump = 0;

		Entity_RandomDirection(entity, STRINGER_BASE_VELOCITY);
	}
}

Entity * Stringer_Create(Game *game){
	Entity *entity = Entity_Create(game);

	entity->texture = game->texture_pack->stringer_texture;

	entity->hitbox_offset = Vec2_Create(6, 6);
	entity->hitbox_size = Vec2_Create(19, 22);

	entity->texture_id = 0;
	entity->entity_type = ENTITY_STRINGER;

	entity->CustomUpdate = Stringer_Update;
	entity->CustomCollider = Stringer_Collider;
	entity->CustomDestructor = EnemyUtil_Destructor;

	entity->properties = malloc(sizeof(StringerProperties));

	StringerProperties *properties = (StringerProperties *) entity->properties;

	properties->is_jumping = 0;
	properties->timer_try = 0;
	properties->timer_jump = 0;
	properties->timer_animation = 0;

	return entity;
}

void Stringer_Update(Game *game, Entity *entity){
	StringerProperties *properties = (StringerProperties *) entity->properties;
	Entity *player = Entity_Find(game, ENTITY_PLAYER);
	int frame;

	if(player == NULL) return;
	
	properties->timer_animation += game->delta_time;
	properties->timer_jump += game->delta_time;
	properties->timer_try += game->delta_time;
	
	frame = (int) ((properties->timer_animation) / SECONDS_PER_FRAME) % 2;

	if(entity->velocity.x > 0){
		entity->texture_id = frame;

		if(properties->is_jumping) entity->texture_id = 2;
	}
	else{
		entity->texture_id = 3 + frame;

		if(properties->is_jumping) entity->texture_id = 5;
	}

	Stringer_ResetJump(entity);

	if(properties->timer_try > TIME_TRY){
		Stringer_Jump(game, entity, player);
		if(properties->is_jumping) properties->timer_try = 0;
	}

	if(Entity_CheckCollision(player, entity)){
		if(properties->is_jumping){
			Entity_DoDamage(player, DAMAGE_WHEN_JUMPING);
			entity->is_dead = 1;
		}
		else{
			Entity_DoDamage(player, DAMAGE_NORMAL);
		}
	}

	Entity_SolveCollisionFor(game, entity, util_enemy_list);
}
