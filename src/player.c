#include "player.h"
#include "input.h"
#include "bullet.h"
#include "world.h"
#include "pickup.h"
#include "sfx.h"
#include "particle.h"

#define PLAYER_VELOCITY 120
#define FRICTION 200
#define SECONDS_PER_FRAME 0.1

#define WPN_COS_ANG 0.9659
#define WPN_SIN_ANG 0.2588

enum Weapons{
	WEAPON_PISTOL = 0,
	WEAPON_SHOTGUN,
	WEAPON_SUB,
	WEAPON_NUM
};

typedef struct{
	Entity *aim;
	double timer_shoot[WEAPON_NUM];
	double timer_animation;

	int current_weapon;

	int num_bullet[WEAPON_NUM];
	int keys[KEY_NUM];
} PlayerProperties;

const double time_to_shoot[] = {0.55, 0.8, 0.1};
const int weapon_damage[] = {34, 57, 34};

const int weapon_pickup_bullet[] = {
	20, // WEAPON_PISTOL
	10, // WEAPON_SHOTGUN
	30  // WEAPON_SUB
};

static void Player_SetUpProperties(Game *game, Entity *player);
static void Player_SetCamera(Game *game, Entity *player);
static void Player_Input(Game *game, Entity *player);
static void Player_ShootShotgun(Game *game, Vec2 hand_position, Vec2 direction);
static void Player_Shoot(Game *game, Entity *player);
static void Player_SetUpVelocity(Game *game, Entity *player);
static void Player_Animate(Entity *player);
static void Player_Destructor(Game *game, Entity *player);

static void Player_SetUpProperties(Game *game, Entity *player){
	PlayerProperties *properties = (PlayerProperties *) player->properties;

	if(properties->aim == NULL){
		properties->aim = Entity_Find(game, ENTITY_AIM);
	}
}

static void Player_SetCamera(Game *game, Entity *player){
	game->camera = Vec2_Sub(
			Entity_GetHitboxCenter(player),
			Vec2_Create(game->internal_width/2 - 4, game->internal_height/2)
			);
	
	if(game->camera.x < 0) game->camera.x = 0;
	if(game->camera.y < 0) game->camera.y = 0;

	if(game->camera.x >= WORLD_SIZE * TILE_SIZE) game->camera.x = WORLD_SIZE * TILE_SIZE - 1;
	if(game->camera.y >= WORLD_SIZE * TILE_SIZE) game->camera.y = WORLD_SIZE * TILE_SIZE - 1;
}

static void Player_Input(Game *game, Entity *player){
	PlayerProperties *properties = (PlayerProperties *) player->properties;

	Vec2 dir = Vec2_Create(0, 0);

	if(Input_IsPressed(&game->context, BUTTON_RIGHT))
		dir.x++;

	if(Input_IsPressed(&game->context, BUTTON_LEFT))
		dir.x--;

	if(Input_IsPressed(&game->context, BUTTON_UP))
		dir.y--;

	if(Input_IsPressed(&game->context, BUTTON_DOWN))
		dir.y++;

	if(Input_IsPressed(&game->context, BUTTON_WPN_PISTOL))
		properties->current_weapon = WEAPON_PISTOL;

	if(Input_IsPressed(&game->context, BUTTON_WPN_SHOTGUN)){
		properties->current_weapon = WEAPON_SHOTGUN;
	}

	if(Input_IsPressed(&game->context, BUTTON_WPN_SUB)){
		properties->current_weapon = WEAPON_SUB;
	}
	
	if(Input_IsControllerConnected(&game->context))
		Input_LeftAnalogDirection(&game->context, &dir.x, &dir.y);

	player->velocity = Vec2_Add(player->velocity, Vec2_Mul(Vec2_Normalize(dir), PLAYER_VELOCITY));
}

static void Player_ShootShotgun(Game *game, Vec2 hand_position, Vec2 direction){
	Vec2 direction_ortho = Vec2_Create(direction.y, -direction.x);
	Vec2 direction_list[3];
	Entity *bullet;

	direction_list[0] = Vec2_Add(
			Vec2_Mul(direction, WPN_COS_ANG),
			Vec2_Mul(direction_ortho, -WPN_SIN_ANG)
			);

	direction_list[1] = direction;

	direction_list[2] = Vec2_Add(
			Vec2_Mul(direction, WPN_COS_ANG),
			Vec2_Mul(direction_ortho, WPN_SIN_ANG)
			);

	for(int i = 0; i < 3; i++){
		bullet = Bullet_Create(game);
		bullet->position = hand_position;

		bullet->velocity = Vec2_Mul(direction_list[i], 500);
		Bullet_SetDamage(bullet, weapon_damage[WEAPON_SHOTGUN]);

		Stack_Push(game->entity_stack, &bullet);
	}
}

static void Player_Shoot(Game *game, Entity *player){
	PlayerProperties *properties = (PlayerProperties *) player->properties;
	Entity *bullet;
	Vec2 hand_position;
	Vec2 direction;

	// Only shoot if there are enough bullets
	if(properties->num_bullet[properties->current_weapon] <= 0){
		properties->num_bullet[properties->current_weapon] = 0;
		return;
	}

	// if it's facing left
	if(player->texture_id >= 6)
		hand_position = Vec2_Add(player->position, Vec2_Create(0, 6));
	else // if it's facing right
		hand_position = Vec2_Add(player->position, Vec2_Create(26, 6));

	if(Input_IsPressed(&game->context, BUTTON_SHOOT) && properties->timer_shoot[properties->current_weapon] > time_to_shoot[properties->current_weapon]){
		properties->timer_shoot[properties->current_weapon] = 0;

		properties->num_bullet[properties->current_weapon]--;

		if(properties->aim != NULL)
			direction = Vec2_Normalize(
					Vec2_Sub(
						Entity_GetHitboxCenter(properties->aim),
						hand_position
						)
					);

		if(properties->current_weapon == WEAPON_SHOTGUN){
			Player_ShootShotgun(game, hand_position, direction);
		}
		else{
			bullet = Bullet_Create(game);
			bullet->position = hand_position;

			bullet->velocity = Vec2_Mul(direction, 500);
			Bullet_SetDamage(bullet, weapon_damage[properties->current_weapon]);

			Stack_Push(game->entity_stack, &bullet);
		}
	}
}

static void Player_SetUpVelocity(Game *game, Entity *player){
	if(Vec2_Length(player->velocity) > PLAYER_VELOCITY){
		player->velocity = Vec2_Mul(Vec2_Normalize(player->velocity), PLAYER_VELOCITY);
	}

	if(Vec2_Length(player->velocity) > 10){
		Vec2 friction = Vec2_Mul(Vec2_Normalize(player->velocity), -FRICTION);
		player->velocity = Vec2_Add(player->velocity, Vec2_Mul(friction, game->delta_time));
	}
	else{
		player->velocity = Vec2_Create(0, 0);
	}
}

static void Player_Animate(Entity *player){
	PlayerProperties *properties = (PlayerProperties *) player->properties;

	if(properties->aim->position.x > player->position.x) player->texture_id = 0;
	else player->texture_id = 6;

	int frame = ((int) (properties->timer_animation / SECONDS_PER_FRAME)) % 2;

	if(Vec2_LengthSqr(player->velocity) > 10)
		player->texture_id += frame;

	player->texture_id += properties->current_weapon * 2;
}

static void Player_Destructor(Game *game, Entity *player){
	if(!player->is_dead) return;

	Entity *particle;

	for(int i = 0; i < 30; i++){
		particle = Particle_Create(game);
		particle->position = Entity_GetHitboxCenter(player);

		Stack_Push(game->entity_stack, &particle);
	}
}

Entity * Player_Create(Game *game){
	Entity *player = Entity_Create(game);
	player->position = Vec2_Create(128, 128);
	player->texture = game->texture_pack->player_texture;
	player->hitbox_size = Vec2_Create(8, 32);
	player->hitbox_offset = Vec2_Create(11, 0);
	player->texture_id = 0;
	player->entity_type = ENTITY_PLAYER;
	player->damage_cooldown = 1.5;
	player->light_when_damaged = 0;
	player->layer = LAYER_PLAYER;

	player->CustomUpdate = Player_Update;
	player->CustomRender = Player_Render;
	player->CustomDestructor = Player_Destructor;

	player->properties = malloc(sizeof(PlayerProperties));

	PlayerProperties *properties = (PlayerProperties *) player->properties;

	properties->aim = NULL;
	properties->timer_animation = 0;
	properties->current_weapon = WEAPON_PISTOL;
	
	for(int i = 0; i < WEAPON_NUM; i++){
		properties->num_bullet[i] = 0;
		properties->timer_shoot[i] = 100;
	}
	
	for(int i = 0; i < KEY_NUM; i++){
		properties->keys[i] = 0;
	}

	return player;
}

int Player_CurrentWeaponBullet(Entity *player){
	PlayerProperties *properties = (PlayerProperties *) player->properties;

	return properties->num_bullet[properties->current_weapon];
}

int Player_HasKey(Entity *player, int key_color){
	PlayerProperties *properties = (PlayerProperties *) player->properties;

	return properties->keys[key_color];
}

void Player_ReceiveHealth(Entity *player, int health){
	player->health += health;

	if(player->health > 100) player->health = 100;
}

void Player_ReceiveKey(Entity *player, int key_color){
	PlayerProperties *properties = (PlayerProperties *) player->properties;

	properties->keys[key_color] = 1;
}

void Player_ReceiveBullet(Entity *player, int type){
	PlayerProperties *properties = (PlayerProperties *) player->properties;

	properties->num_bullet[type] += weapon_pickup_bullet[type];
}

void Player_Update(Game *game, Entity *player){
	PlayerProperties *properties = (PlayerProperties *) player->properties;

	Player_SetUpProperties(game, player);

	properties->timer_shoot[properties->current_weapon] += game->delta_time;
	properties->timer_animation += game->delta_time;
	
	Player_SetUpProperties(game, player);

	Player_SetCamera(game, player);

	Player_Shoot(game, player);

	Player_Input(game, player);
	
	Player_SetUpVelocity(game, player);

	Player_Animate(player);

	if(player->was_damaged) Sfx_Play(game->texture_pack->enemydamage_sfx, 0, 1);
}

void Player_Render(Game *game, Entity *player){
	Game_SetDrawLayer(game, LAYER_SHADOW);

	Texture_RenderCell(
			&game->context,
			game->texture_pack->player_shadow_texture,
			player->position.x - game->camera.x + 1,
			player->position.y - game->camera.y,
			-1
			);
}
