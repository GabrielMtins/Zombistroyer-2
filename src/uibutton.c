#include "uibutton.h"
#include "input.h"

typedef struct{
	int texture;
	int *clicked;
} UiButtonProperties;

Entity * UiButton_Create(Game *game, int texture, int *clicked){
	Entity *entity = Entity_Create(game);

	entity->is_rigid = 0;
	entity->hitbox_size = Vec2_Create(128, 32);
	entity->texture = game->texture_pack->uibutton_texture;
	entity->texture_id = texture;

	entity->CustomUpdate = UiButton_Update;
	entity->properties = malloc(sizeof(UiButtonProperties));
	entity->entity_type = ENTITY_UIBUTTON;

	UiButtonProperties *properties = (UiButtonProperties *) entity->properties;

	properties->clicked = clicked;
	properties->texture = texture;

	return entity;
}

void UiButton_Update(Game *game, Entity *entity){
	UiButtonProperties *properties = (UiButtonProperties *) entity->properties;
	Entity *aim = Entity_Find(game, ENTITY_AIM);

	if(aim == NULL) return;

	if(Entity_CheckCollision(aim, entity)){
		if(Input_IsPressed(&game->context, BUTTON_SHOOT))
			*(properties->clicked) = 1;
		else
			*(properties->clicked) = 0;

		entity->texture_id = properties->texture + 1;
	}
	else{
		entity->texture_id = properties->texture;
		*(properties->clicked) = 0;
	}
}

