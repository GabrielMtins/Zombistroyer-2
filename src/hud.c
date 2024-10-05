#include "hud.h"
#include "world.h"
#include "aim.h"
#include "uibutton.h"
#include "draw.h"
#include "pickup.h"
#include "player.h"
#include "dialogbox.h"
#include "input.h"

enum Scenes{
	SCENE_MAIN_MENU = 0,
	SCENE_PLAY,
	SCENE_GAMEOVER,
	SCENE_YOUWIN,
	SCENE_BEATEN
};

typedef struct{
	int scene;
	int level;
	int num_enemies;
	int enemies_killed;

	struct{
		int play;
		int quit;
	} signal;

	struct{
		double timer;
	} mainmenu;

	struct{
		double timer;
	} gameover;

	struct{
		double timer;
	} youwin;
} HudProperties;

static const char *levels[] = {
	"materials/maps/map00.zm",
	"materials/maps/map01.zm",
	"materials/maps/map02.zm",
	"materials/maps/map03.zm",
	"materials/maps/map04.zm",
	NULL
};

static const int white[] = {255, 255, 255, 255};
static const int black[] = {0, 0, 0, 255};
static const int black_transparent[] = {30, 30, 30, 180};

static double Hud_Smooth(double i);
static void Hud_SetUpMainMenu(Game *game, Entity *hud);
static void Hud_LoadNextLevel(Game *game, Entity *hud);
static void Hud_DrawPlayHud(Game *game, Entity *hud);
static void Hud_DrawMainMenu(Game *game, Entity *hud);
static void Hud_DrawGameOver(Game *game, Entity *hud);
static void Hud_DrawYouWin(Game *game, Entity *hud);
static void Hud_DrawYouBeat(Game *game, Entity *hud);

static double Hud_Smooth(double i){
	if(i > 1) return 1;
	if(i < 0) return 0;
	return ((6*i - 15)*i + 10)*i*i*i;
}

static void Hud_SetUpMainMenu(Game *game, Entity *hud){
	HudProperties *properties = (HudProperties *) hud->properties;

	properties->scene = SCENE_MAIN_MENU;
	properties->level = -1;
	properties->signal.play = 0;
	properties->signal.quit = 0;
	properties->mainmenu.timer = 0;

	Entity_Destroy(game, game->world);

	Game_CleanEntityStack(game);
	game->camera = Vec2_Create(0, 0);
	game->world = World_Create(game);

	Sfx_Play(game->texture_pack->mainmenu_music, -1, -1);
}

static void Hud_LoadNextLevel(Game *game, Entity *hud){
	HudProperties *properties = (HudProperties *) hud->properties;
	Entity **tmp;
	int type;

	properties->scene = SCENE_PLAY;
	properties->level++;
	properties->num_enemies = 0;
	properties->enemies_killed = 0;

	if(levels[properties->level] == NULL){
		properties->level--;
		properties->scene = SCENE_BEATEN;
		Sfx_Play(game->texture_pack->mainmenu_music, -1, -1);
		return;
	}

	World_Load(game, game->world, levels[properties->level]);

	Sfx_Play(game->texture_pack->level_theme[properties->level], -1, -1);

	for(size_t i = 0; i < game->entity_stack->size; i++){
		tmp = Stack_Get(game->entity_stack, i);
		type = (*tmp)->entity_type;
		
		switch(type){
			case ENTITY_ZOMBIE:
			case ENTITY_SHOOTER:
			case ENTITY_FIXER:
			case ENTITY_STRINGER:
			case ENTITY_SHOTGUN:
				properties->num_enemies++;
				break;
		}
	}
}

static void Hud_DrawPlayHud(Game *game, Entity *hud){
	char status[20];
	Entity *player;
	HudProperties *properties = (HudProperties *) hud->properties;

	if(properties->scene != SCENE_PLAY) return;

	player = Entity_Find(game, ENTITY_PLAYER);

	if(player == NULL){
		return;
	}

	Game_SetDrawLayer(game, LAYER_HUD);

	/* draw player health */
	sprintf(status, "Health: %3d", player->health);

	Draw_DrawTextWithBox(
			&game->context,
			8,
			game->internal_height - 24,
			4,
			black_transparent,
			white,
			status
			);

	/* draw player ammo */
	sprintf(status, "Ammo: %3d", Player_CurrentWeaponBullet(player));

	Draw_DrawTextWithBox(
			&game->context,
			game->internal_width - 60,
			game->internal_height - 24,
			4,
			black_transparent,
			white, 
			status
			);

	for(int i = 0; i < KEY_NUM; i++){
		if(!Player_HasKey(player, i)) continue;

		Texture_RenderCell(
				&game->context,
				game->texture_pack->pickup_texture,
				4 + i * 14,
				4,
				i
				);
	}
}

static void Hud_DrawMainMenu(Game *game, Entity *hud){
	HudProperties *properties = (HudProperties *) hud->properties;
	double time_to_place, text_position;
	double min_time_first = 0.05;
	double min_time_second = 0.20;
	double min_time_third = 0.40;
	int rect[4] = {0, 0, game->internal_width, game->internal_height};

	if(properties->scene != SCENE_MAIN_MENU) return;

	Draw_DrawRect(&game->context, rect, black);

	time_to_place = 0.8;

	text_position =
		Hud_Smooth((properties->mainmenu.timer - min_time_first) / time_to_place) * 60;

	Draw_DrawTextWithBoxCentered(
			&game->context,
			game->internal_width / 2,
			-20 + text_position,
			4,
			black_transparent,
			white,
			"Zombistroyer 2"
			);

	text_position =
		Hud_Smooth((properties->mainmenu.timer - min_time_second) / time_to_place) * 50;

	if(!Input_IsControllerConnected(&game->context)){
		Draw_DrawTextCentered(
				&game->context,
				game->internal_width / 2,
				game->internal_height - text_position + 10,
				white,
				"Press the left mouse button to start."
				);
	}
	else{
		Draw_DrawTextCentered(
				&game->context,
				game->internal_width / 2,
				game->internal_height - text_position + 10,
				white,
				"Press the right trigger button to start."
				);
	}

	text_position =
		Hud_Smooth((properties->mainmenu.timer - min_time_third) / time_to_place) * 50;

	Draw_DrawTextCentered(
			&game->context,
			game->internal_width / 2,
			game->internal_height - text_position + 30,
			white,
			"Developed by Gabriel Martins."
			);
}

static void Hud_DrawGameOver(Game *game, Entity *hud){
	HudProperties *properties = (HudProperties *) hud->properties;
	double time_to_place, game_over_text_position;
	double min_time_first = 0.05;
	double min_time_second = 0.20;

	if(properties->scene != SCENE_GAMEOVER) return;

	time_to_place = 0.8;

	game_over_text_position =
		Hud_Smooth((properties->gameover.timer - min_time_first) / time_to_place) * 60;

	Draw_DrawTextWithBoxCentered(
			&game->context,
			game->internal_width / 2,
			-20 + game_over_text_position,
			4,
			black_transparent,
			white,
			"Game Over"
			);

	game_over_text_position =
		Hud_Smooth((properties->gameover.timer - min_time_second) / time_to_place) * 50;

	Draw_DrawTextWithBoxCentered(
			&game->context,
			game->internal_width / 2,
			game->internal_height - game_over_text_position + 20,
			4,
			black_transparent,
			white,
			"Press Fire to try again"
			);
}

static void Hud_DrawYouWin(Game *game, Entity *hud){
	HudProperties *properties = (HudProperties *) hud->properties;
	double time_to_place, game_over_text_position;
	double min_time_first = 0.05;
	double min_time_second = 0.20;
	double min_time_third = 0.30;
	char enemies_killed_text[80];

	if(properties->scene != SCENE_YOUWIN) return;

	time_to_place = 0.8;

	game_over_text_position =
		Hud_Smooth((properties->youwin.timer - min_time_first) / time_to_place) * 60;

	Draw_DrawTextWithBoxCentered(
			&game->context,
			game->internal_width / 2,
			-20 + game_over_text_position,
			4,
			black_transparent,
			white,
			"You got the gold key!"
			);

	game_over_text_position =
		Hud_Smooth((properties->youwin.timer - min_time_second) / time_to_place) * 80;

	Draw_DrawTextWithBoxCentered(
			&game->context,
			game->internal_width / 2,
			game->internal_height - game_over_text_position + 20,
			4,
			black_transparent,
			white,
			"Press Fire to play the next level"
			);

	game_over_text_position =
		Hud_Smooth((properties->youwin.timer - min_time_third) / time_to_place) * 50;

	sprintf(enemies_killed_text, "Enemies killed: %d/%d", properties->enemies_killed, properties->num_enemies);

	Draw_DrawTextWithBoxCentered(
			&game->context,
			game->internal_width / 2,
			game->internal_height - game_over_text_position + 20,
			4,
			black_transparent,
			white,
			enemies_killed_text
			);
}

static void Hud_DrawYouBeat(Game *game, Entity *hud){
	HudProperties *properties = (HudProperties *) hud->properties;
	double time_to_place, game_over_text_position;
	double min_time_first = 0.05;
	double min_time_second = 0.20;
	int rect[4] = {0, 0, game->internal_width, game->internal_height};

	if(properties->scene != SCENE_BEATEN) return;

	Draw_DrawRect(&game->context, rect, black);

	time_to_place = 0.8;

	game_over_text_position =
		Hud_Smooth((properties->youwin.timer - min_time_first) / time_to_place) * 60;

	Draw_DrawTextWithBoxCentered(
			&game->context,
			game->internal_width / 2,
			-20 + game_over_text_position,
			4,
			black_transparent,
			white,
			"Congratulations, you've beaten the game!"
			);

	game_over_text_position =
		Hud_Smooth((properties->youwin.timer - min_time_second) / time_to_place) * 50;

	Draw_DrawTextWithBoxCentered(
			&game->context,
			game->internal_width / 2,
			game->internal_height - game_over_text_position + 20,
			4,
			black_transparent,
			white,
			"Press Fire to play go back to the main menu"
			);
}

Entity * Hud_Create(Game *game){
	Entity *hud = Entity_Create(game);

	hud->layer = ENTITY_HUD;
	hud->entity_type = ENTITY_HUD;
	hud->CustomUpdate = Hud_Update;
	hud->CustomRender = Hud_Render;

	hud->properties = malloc(sizeof(HudProperties));

	Hud_SetUpMainMenu(game, hud);

	return hud;
}

void Hud_AddToEnemyCounter(Entity *hud){
	HudProperties *properties = (HudProperties *) hud->properties;

	properties->enemies_killed++;
}

void Hud_Update(Game *game, Entity *hud){
	HudProperties *properties = (HudProperties *) hud->properties;
	Entity *player;

	if(properties->scene == SCENE_MAIN_MENU){
		properties->mainmenu.timer += game->delta_time;

		if(properties->mainmenu.timer > 0.5 && Input_IsPressed(&game->context, BUTTON_SHOOT)){
			Hud_LoadNextLevel(game, hud);
			properties->youwin.timer = 0;
		}
	}
	else if(properties->scene == SCENE_PLAY){
		player = Entity_Find(game, ENTITY_PLAYER);

		if(Entity_Find(game, ENTITY_PLAYER) == NULL){
			properties->scene = SCENE_GAMEOVER;
			properties->gameover.timer = 0;
		}
		else if(Player_HasKey(player, KEY_GOLD)){
			properties->scene = SCENE_YOUWIN;
			properties->youwin.timer = 0;
		}
	}
	else if(properties->scene == SCENE_GAMEOVER){
		properties->gameover.timer += game->delta_time;

		if(properties->gameover.timer >= 0.8 && Input_IsPressed(&game->context, BUTTON_SHOOT)){
			properties->level--;
			Hud_LoadNextLevel(game, hud);
		}
	}
	else if(properties->scene == SCENE_YOUWIN){
		properties->youwin.timer += game->delta_time;

		if(properties->youwin.timer >= 1.2 && Input_IsPressed(&game->context, BUTTON_SHOOT)){
			Hud_LoadNextLevel(game, hud);
			properties->youwin.timer = 0;
		}
	}
	else if(properties->scene == SCENE_BEATEN){
		properties->youwin.timer += game->delta_time;

		if(properties->youwin.timer >= 1.2 && Input_IsPressed(&game->context, BUTTON_SHOOT)){
			Hud_SetUpMainMenu(game, hud);
		}
	}
}

void Hud_Render(Game *game, Entity *hud){
	Game_SetDrawLayer(game, LAYER_HUD);
	Hud_DrawMainMenu(game, hud);
	Hud_DrawPlayHud(game, hud);
	Hud_DrawGameOver(game, hud);
	Hud_DrawYouWin(game, hud);
	Hud_DrawYouBeat(game, hud);
}
