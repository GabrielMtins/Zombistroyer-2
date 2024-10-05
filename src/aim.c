#include "aim.h"

#include "input.h"

typedef struct{
	Vec2 dir;
} AimProperties;

Entity * Aim_Create(Game *game){
	Entity *entity = Entity_Create(game);
	entity->texture = game->texture_pack->aim_texture;
	entity->entity_type = ENTITY_AIM;
	entity->layer = LAYER_AIM;
	entity->is_rigid = 0;
	entity->texture_id = -1;
	entity->hitbox_size = Vec2_Create(12, 12);

	entity->CustomUpdate = Aim_Update;

	entity->properties = malloc(sizeof(AimProperties));

	AimProperties *properties = (AimProperties *) entity->properties;
	properties->dir = Vec2_Create(1, 0);

	return entity;
}

void Aim_Update(Game *game, Entity *entity){
	AimProperties *properties = (AimProperties *) entity->properties;

	if(Input_IsControllerConnected(&game->context)){
		Vec2 dir;
		Input_RightAnalogDirection(&game->context, &dir.x, &dir.y);

		if(Vec2_LengthSqr(dir) != 0){
			properties->dir = Vec2_Normalize(dir);
		}

		dir = properties->dir;

		entity->position = Vec2_Add(
				Vec2_Create(
					game->camera.x + game->internal_width / 2 - 6,
					game->camera.y + game->internal_height / 2 - 14
					),
				Vec2_Mul(dir, 40)
				);
	}
	else{
		double x, y;
	
		Input_GetMousePosition(&game->context, &x, &y);
		x *= game->internal_width;
		y *= game->internal_height;

		entity->position = Vec2_Create(x - 6 + game->camera.x, y - 6 + game->camera.y);
	}
}
