#include "pickup.h"
#include "player.h"
#include "sfx.h"

enum PickupType{
	PICKUP_KEY = 0,
	PICKUP_BULLET,
	PICKUP_HEALTH,
};

typedef struct{
	int type;
	int color;

	int pickup_type;
} PickupProperties;

Entity * Pickup_Create(Game *game){
	Entity *entity = Entity_Create(game);
	
	entity->texture = game->texture_pack->pickup_texture;
	entity->hitbox_size = Vec2_Create(16, 16);
	entity->texture_id = 0;
	entity->entity_type = ENTITY_PICKUP;
	entity->layer = LAYER_PICKUP;

	entity->properties = malloc(sizeof(PickupProperties));

	entity->CustomUpdate = Pickup_Update;

	return entity;
}

Entity * Pickup_CreateKey(Game *game, int color){
	Entity *entity = Pickup_Create(game);

	entity->texture_id = color;

	PickupProperties *properties = (PickupProperties *) entity->properties;

	properties->pickup_type = PICKUP_KEY;
	properties->color = color;

	return entity;
}

Entity * Pickup_CreateHealthKit(Game *game){
	Entity *entity = Pickup_Create(game);

	entity->texture_id = 7;
	PickupProperties *properties = (PickupProperties *) entity->properties;

	properties->pickup_type = PICKUP_HEALTH;

	return entity;
}

Entity * Pickup_CreateBulletKit(Game *game, int type){
	Entity *entity = Pickup_Create(game);

	entity->texture_id = 4 + type;

	PickupProperties *properties = (PickupProperties *) entity->properties;

	properties->pickup_type = PICKUP_BULLET;
	properties->type = type;

	return entity;
}

void Pickup_Update(Game *game, Entity *entity){
	Entity *player = Entity_Find(game, ENTITY_PLAYER);
	PickupProperties *properties = (PickupProperties *) entity->properties;

	/* Only continues if it has found a collision; */
	if(player == NULL) return;
	if(!Entity_CheckCollision(player, entity)) return;

	switch(properties->pickup_type){
		case PICKUP_KEY:
			Player_ReceiveKey(player, properties->color);
			//if(properties->color == KEY_GOLD) Sfx_FadeOutMusic(500);
			break;

		case PICKUP_BULLET:
			Player_ReceiveBullet(player, properties->type);
			break;

		case PICKUP_HEALTH:
			Player_ReceiveHealth(player, 20);
			break;
	}

	entity->is_dead = 1;
	Sfx_Play(game->texture_pack->collectkey_sfx, 0, -1);
}
