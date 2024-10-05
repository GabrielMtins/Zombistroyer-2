#include "entity.h"
#include "world.h"

#include <math.h>

#define DAMAGE_COOLDOWN_BLINK 0.1

Entity * Entity_Create(Game *game){
	Entity *entity = (Entity *) malloc(sizeof(Entity));
	
	entity->position = Vec2_Create(0, 0);
	entity->velocity = Vec2_Create(0, 0);
	entity->hitbox_size = Vec2_Create(24, 24);
	entity->hitbox_offset = Vec2_Create(0, 0);

	entity->is_rigid = 1;

	entity->health = 100;

	entity->entity_type = ENTITY_NONE;
	entity->texture = NULL;
	entity->texture_id = -1;
	entity->CustomUpdate = NULL;
	entity->CustomRender = NULL;
	entity->CustomCollider = NULL;
	entity->CustomDestructor = NULL;
	entity->layer = LAYER_SPRITE;
	entity->is_dead = 0;
	entity->light_when_damaged = 1;
	entity->was_damaged = 0;

	entity->damage_timer = 100;
	entity->damage_cooldown = 0.1;

	entity->properties = NULL;

	return entity;
}

void Entity_Update(Game *game, Entity *entity){
	if(entity->health <= 0) entity->is_dead = 1;
	entity->damage_timer += game->delta_time;

	entity->position = Vec2_Add(
			entity->position,
			Vec2_Mul(entity->velocity, game->delta_time)
			);

	if(entity->is_rigid){
		if(World_SolveCollisionForEntity(game->world, entity)){
			if(entity->CustomCollider != NULL){
				entity->CustomCollider(game, entity);
			}
		}
	}

	if(entity->CustomUpdate != NULL){
		entity->CustomUpdate(game, entity);
	}

	entity->was_damaged = 0;
}

void Entity_Render(Game *game, Entity *entity){
	Game_SetDrawLayer(game, entity->layer);

	void (*Texture_Render)(Context *, Texture *, int, int, int);

	Texture_Render = Texture_RenderCell;

	if(entity->damage_timer < entity->damage_cooldown){
		int frame = (entity->damage_timer) / (DAMAGE_COOLDOWN_BLINK);

		if(entity->light_when_damaged)
			Texture_Render = Texture_RenderCellLighted;
		else if(frame % 2 == 0)
			return;
	}

	if(entity->CustomRender != NULL){
		entity->CustomRender(game, entity);
	}

	if(entity->texture != NULL){
		Game_SetDrawLayer(game, entity->layer);

		Texture_Render(
				&game->context,
				entity->texture,
				floor(entity->position.x - game->camera.x),
				floor(entity->position.y - game->camera.y),
				entity->texture_id
				);
	}
}

int Entity_CheckCollision(Entity *e1, Entity *e2){
	Vec2 pos1 = Vec2_Add(e1->position, e1->hitbox_offset);
	Vec2 pos2 = Vec2_Add(e2->position, e2->hitbox_offset);

	if(pos1.x + e1->hitbox_size.x <= pos2.x) return 0;
	if(pos1.y + e1->hitbox_size.y <= pos2.y) return 0;
	if(pos1.x > pos2.x + e2->hitbox_size.x) return 0;
	if(pos1.y > pos2.y + e2->hitbox_size.y) return 0;

	return 1;
}

void Entity_SolveCollision(Entity *e1, Entity *e2){
	Vec2 pos1 = Vec2_Add(e1->position, e1->hitbox_offset);
	Vec2 pos2 = Vec2_Add(e2->position, e2->hitbox_offset);

	if(!Entity_CheckCollision(e1, e2)) return;

	double dx =
		fmin(pos1.x + e1->hitbox_size.x, pos2.x + e2->hitbox_size.x)
		-
		fmax(pos1.x, pos2.x);

	double dy =
		fmin(pos1.y + e1->hitbox_size.y, pos2.y + e2->hitbox_size.y)
		-
		fmax(pos1.y, pos2.y);

	if(fabs(dx) < fabs(dy)){
		if(pos1.x < pos2.x)
			e1->position.x -= dx;
		else
			e1->position.x += dx;
	}
	else{
		if(pos1.y < pos2.y)
			e1->position.y -= dy;
		else
			e1->position.y += dy - 1;
	}
}

int Entity_DoDamage(Entity *entity, int damage){
	if(entity->light_when_damaged || entity->damage_cooldown == 0 || entity->damage_timer >= entity->damage_cooldown){
		entity->health -= damage;
		entity->damage_timer = 0;
		entity->was_damaged = 1;

		return 1;
	}

	return 0;
}

Entity * Entity_Find(Game *game, int entity_type){
	Entity **entity = NULL;

	for(size_t i = 0; i < game->entity_stack->size; i++){
		entity = Stack_Get(game->entity_stack, i);

		if((*entity)->entity_type == entity_type){
			return *entity;
		}
	}

	return NULL;
}

void Entity_RandomDirection(Entity *entity, double velocity){
	Vec2 random_vector = Vec2_Create(
			(double) (rand() % 101 - 50) / 50,
			(double) (rand() % 101 - 50) / 50
			);

	random_vector = Vec2_Normalize(random_vector);

	entity->velocity = Vec2_Mul(random_vector, velocity);
}

Vec2 Entity_GetHitboxCenter(Entity *entity){
	Vec2 center = Vec2_Add(entity->position, entity->hitbox_offset);
	
	center = Vec2_Add(
			center,
			Vec2_Mul(entity->hitbox_size, 0.5)
			);

	return center;
}

double Entity_DistSqrBetween(Entity *a, Entity *b){
	return Vec2_LengthSqr(Vec2_Sub(
			Entity_GetHitboxCenter(a),
			Entity_GetHitboxCenter(b)
			));
}

void Entity_SolveCollisionFor(Game *game, Entity *entity, const int *list){
	Entity **tmp;
	Vec2 original_position;

	for(size_t i = 0; i < game->entity_stack->size; i++){
		tmp = Stack_Get(game->entity_stack, i);

		if(*tmp == entity) continue;

		if(!Entity_CheckCollision(entity, *tmp)) continue;

		for(int j = 0; list[j] != ENTITY_NONE; j++){
			if((*tmp)->entity_type == list[j]){
				original_position = entity->position;

				Entity_SolveCollision(entity, *tmp);

				if(World_SolveCollisionForEntity(game->world, entity))
					entity->position = original_position;

				break;
			}
		}
	}
}

void Entity_Destroy(Game *game, Entity *entity){
	if(entity == NULL) return;

	if(entity->CustomDestructor != NULL)
		entity->CustomDestructor(game, entity);

	if(entity->properties != NULL){
		free(entity->properties);
	}

	free(entity);
}
