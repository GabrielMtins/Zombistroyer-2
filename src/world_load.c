#include "world_load.h"
#include "entity.h"
#include "player.h"
#include "world.h"
#include "aim.h"
#include "zombie.h"
#include "shooter.h"
#include "fixer.h"
#include "stringer.h"
#include "shotgun.h"
#include "pickup.h"
#include "door.h"
#include "dialogbox.h"

void WorldLoad_LoadEntity(Game *game, int id, int i, int j){
	Entity *entity = NULL;

	switch(id){
		case 128:
			entity = Player_Create(game);
			break;

		case 129:
			entity = Zombie_Create(game);
			break;

		case 130:
			entity = Shooter_Create(game);
			break;

		case 131:
			entity = Fixer_Create(game);
			break;

		case 132:
			entity = Stringer_Create(game);
			break;

		case 133:
			entity = Shotgun_Create(game);
			break;

		case 134:
		case 135:
		case 136:
			entity = Door_Create(game, id - 134);
			break;
		
		case 137:
			entity = DialogTrigger_Create(game);
			break;

		case 160:
		case 161:
		case 162:
		case 163:
			entity = Pickup_CreateKey(game, id - 160);
			break;

		case 164:
		case 165:
		case 166:
			entity = Pickup_CreateBulletKit(game, id - 164);
			break;

		case 167:
			entity = Pickup_CreateHealthKit(game);
			break;
	}

	entity->position = Vec2_Create(i * TILE_SIZE, j * TILE_SIZE);

	Stack_Push(game->entity_stack, &entity);
}
