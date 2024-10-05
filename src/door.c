#include "door.h"
#include "pickup.h"
#include "player.h"

typedef struct{
	int color;
} DoorProperties;

static const int enemy_list[] = {
	ENTITY_ZOMBIE,
	ENTITY_SHOOTER,
	ENTITY_FIXER,
	ENTITY_STRINGER,
	ENTITY_SHOTGUN,
	ENTITY_NONE // mark the end of the list
};

static void Door_IntersectEnemy(Game *game, Entity *entity);

static void Door_IntersectEnemy(Game *game, Entity *entity){
	Entity **tmp;

	for(size_t i = 0; i < game->entity_stack->size; i++){
		tmp = Stack_Get(game->entity_stack, i);

		for(int j = 0; enemy_list[j] != ENTITY_NONE; j++){
			if((*tmp)->entity_type == enemy_list[j] && Entity_CheckCollision(*tmp, entity)){
				Entity_SolveCollision(*tmp, entity);
				if((*tmp)->CustomCollider != NULL){
					(*tmp)->CustomCollider(game, *tmp);
				}
			}
		}
	}
}

Entity * Door_Create(Game *game, int color){
	Entity *entity = Entity_Create(game);
	
	entity->texture = game->texture_pack->door_texture;
	entity->hitbox_size = Vec2_Create(11, 64);
	entity->hitbox_offset = Vec2_Create(3, 0);
	entity->is_rigid = 0;
	
	entity->texture_id = color;
	entity->entity_type = ENTITY_DOOR;

	entity->properties = malloc(sizeof(DoorProperties));
	DoorProperties *properties = (DoorProperties *) entity->properties;

	properties->color = color;

	entity->CustomUpdate = Door_Update;
	entity->CustomRender = Door_Render;
	
	return entity;
}

void Door_Update(Game *game, Entity *entity){
	DoorProperties *properties = (DoorProperties *) entity->properties;

	Entity *player = Entity_Find(game, ENTITY_PLAYER);
	entity->health = 100;

	Door_IntersectEnemy(game, entity);

	if(player == NULL) return;
	if(!Entity_CheckCollision(player, entity)) return;

	if(Player_HasKey(player, properties->color)){
		entity->is_dead = 1;
		Sfx_Play(game->texture_pack->dooropen_sfx , 0, -1);
	}
	else Entity_SolveCollision(player, entity);
}

void Door_Render(Game *game, Entity *entity){
	Game_SetDrawLayer(game, LAYER_SHADOW);

	Texture_RenderCell(
			&game->context,
			game->texture_pack->door_shadow_texture,
			entity->position.x - game->camera.x,
			entity->position.y - game->camera.y,
			-1
			);
}
