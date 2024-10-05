#include "world.h"
#include "world_load.h"
#include "aim.h"
#include "dialogbox.h"

static int World_GetRealId(int id);

static int World_GetRealId(int id){
	if(id == -1) return id;

	int texture_length = 16;
	int texture_displacement = texture_length * 2;

	int line =  (id / texture_displacement) * texture_length;

	return line + id % texture_length;
}

Entity * World_Create(Game *game){
	Entity *entity = Entity_Create(game);

	entity->texture = game->texture_pack->world_texture;
	entity->properties = malloc(sizeof(WorldProperties));

	WorldProperties *properties = (WorldProperties *) entity->properties;

	for(int i = 0; i < WORLD_SIZE; i++){
		for(int j = 0; j < WORLD_SIZE; j++){
			properties->tiles_fg[i][j] = -1;
			properties->tiles_bg[i][j] = -1;
		}
	}

	return entity;
}

void World_Load(Game *game, Entity *world, const char *filename){
	WorldProperties *properties = (WorldProperties *) world->properties;

	FILE *file = fopen(filename, "r");
	int id_real;
	int id_fake;

	if(file == NULL){
		fprintf(stderr, "Failed to load world: %s\n", filename);
		return;
	}

	Game_CleanEntityStack(game);

	/* Load background tiles */
	for(int j = 0; j < WORLD_SIZE; j++){
		for(int i = 0; i < WORLD_SIZE; i++){
			fscanf(file, "%d", &id_fake);

			id_real = World_GetRealId(id_fake);

			properties->tiles_bg[i][j] = id_real;
		}
	}

	/* Load foreground tiles */
	for(int j = 0; j < WORLD_SIZE; j++){
		for(int i = 0; i < WORLD_SIZE; i++){
			fscanf(file, "%d", &id_fake);

			id_real = World_GetRealId(id_fake);

			if(id_real < 128){
				properties->tiles_fg[i][j] = id_real;
			}
			else{
				properties->tiles_fg[i][j] = -1;
				WorldLoad_LoadEntity(game, id_real, i, j);
			}
		}
	}

	/* push an aim to the game */
	Entity *aim = Aim_Create(game);

	Stack_Push(game->entity_stack, &aim);

	Entity *dialogbox = DialogBox_Create(game);

	Stack_Push(game->entity_stack, &dialogbox);

	fclose(file);
}

int World_GetTileFG(Entity *world, int i, int j){
	if(i < 0 || j < 0 || i >= WORLD_SIZE || j >= WORLD_SIZE) return 0;
	WorldProperties *properties = (WorldProperties *) (world->properties);

	return properties->tiles_fg[i][j];
}

int World_GetTileBG(Entity *world, int i, int j){
	if(i < 0 || j < 0 || i >= WORLD_SIZE || j >= WORLD_SIZE) return 0;
	WorldProperties *properties = (WorldProperties *) (world->properties);

	return properties->tiles_bg[i][j];
}

int World_CanEntitySee(Entity *world, Entity *entity, Vec2 point){
	Vec2 pos = Entity_GetHitboxCenter(entity);
	double entity_intx = (int) pos.x / TILE_SIZE;
	double entity_inty = (int) pos.y / TILE_SIZE;
	double point_intx  = (int) point.x / TILE_SIZE;
	double point_inty  = (int) point.y / TILE_SIZE;

	double dx = point_intx - entity_intx;
	double dy = point_inty - entity_inty;
	double steps;

	if(fabs(dx) > fabs(dy)) steps = fabs(dx);
	else steps = fabs(dy);

	for(int i = 0; i < steps; i++){
		if(World_GetTileFG(world, entity_intx, entity_inty) != -1)
			return 0;

		entity_intx += dx / steps;
		entity_inty += dy / steps;
	}

	return 1;
}

int World_SolveCollisionForEntity(Entity *world, Entity *entity){
	int found = 0;
	int world_intx, world_inty;

	Vec2 og_pos = Vec2_Sub(
			Vec2_Add(entity->position, entity->hitbox_offset),
			Vec2_Create(
				(int) (entity->position.x + entity->hitbox_offset.x) % TILE_SIZE,
				(int) (entity->position.y + entity->hitbox_offset.y) % TILE_SIZE
				)
			);

	world->hitbox_size = Vec2_Create(TILE_SIZE, TILE_SIZE);

	for(int i = 0; i < (entity->hitbox_size.x / TILE_SIZE) + 1; i++){
		for(int j = 0; j < (entity->hitbox_size.y / TILE_SIZE) + 1; j++){
			world->position = Vec2_Add(og_pos, Vec2_Create(i * TILE_SIZE, j * TILE_SIZE));

			world_intx = world->position.x / TILE_SIZE;
			world_inty = world->position.y / TILE_SIZE;

			if(World_GetTileFG(world, world_intx, world_inty) == -1)
				continue;

			if(Entity_CheckCollision(entity, world)){
				Entity_SolveCollision(entity, world);
				found = 1;
			}
		}
	}

	return found;
}

void World_Render(Game *game, Entity *world){
	if(world == NULL) return;

	WorldProperties *properties = (WorldProperties *) world->properties;

	int min_x = game->camera.x / TILE_SIZE - 2;
	int min_y = game->camera.y / TILE_SIZE - 2;
	int max_x = min_x + game->internal_width / TILE_SIZE + 4;
	int max_y = min_y + game->internal_height / TILE_SIZE + 4;

	Game_SetDrawLayer(game, LAYER_BACKGROUND);

	for(int i = min_x; i < max_x; i++){
		for(int j = min_y; j < max_y; j++){
			if(i < 0 || j < 0 || i >= WORLD_SIZE || j >= WORLD_SIZE) continue;

			if(properties->tiles_bg[i][j] != -1){
				Texture_RenderCell(
						&game->context,
						world->texture,
						i * TILE_SIZE - game->camera.x,
						j * TILE_SIZE - game->camera.y,
						properties->tiles_bg[i][j]
						);
			}
		}
	}

	Game_SetDrawLayer(game, LAYER_FOREGROUND);

	for(int i = min_x; i < max_x; i++){
		for(int j = min_y; j < max_y; j++){
			if(i < 0 || j < 0 || i >= WORLD_SIZE || j >= WORLD_SIZE) continue;

			if(properties->tiles_fg[i][j] != -1){
				Texture_RenderCell(
						&game->context,
						world->texture,
						i * TILE_SIZE - game->camera.x,
						j * TILE_SIZE - game->camera.y,
						properties->tiles_fg[i][j]
						);
			}
		}
	}

	Game_SetDrawLayer(game, LAYER_SHADOW);

	for(int i = min_x; i < max_x; i++){
		for(int j = min_y; j < max_y; j++){
			if(i < 0 || j < 0 || i >= WORLD_SIZE || j >= WORLD_SIZE) continue;

			if(properties->tiles_fg[i][j] != -1){
				Texture_RenderCell(
						&game->context,
						game->texture_pack->shadow_texture,
						i * TILE_SIZE - game->camera.x,
						j * TILE_SIZE - game->camera.y + TILE_SIZE,
						-1
						);
			}
		}
	}
}
