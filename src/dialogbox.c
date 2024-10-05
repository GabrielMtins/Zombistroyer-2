#include "dialogbox.h"
#include "draw.h"
#include "player.h"
#include "pickup.h"
#include "input.h"
#include <string.h>

#define TIME_PER_CHAR 0.08

static const char *tutorial_dialog_gamepad[] = {
	"It looks like I was the only one to survive. I can move and aim with the analog stick.",
	"I can press left to select the pistol and shoot with right trigger button.",
	"I can change to other weapons by pressing down or right on the dpad.",
	"I can open a colored door if I have its colored key.",
	"I need to get the golden keys to get out of this place.",
	NULL
};

static const char *tutorial_dialog[] = {
	"It looks like I was the only one to survive. I can move with WASD.",
	"I can select the pistol by pressing 1 and firing it with the left mouse button.",
	"I can change to other weapons by pressing 2 or 3.",
	"I can open a colored door if I have its colored key.",
	"I need to get the golden keys to get out of this place.",
	NULL
};

typedef struct{
	double time_alive;
	int current_char;
	int show;
	int text_index;
	char **text;
} DialogBoxProperties;

Entity * DialogBox_Create(Game *game){
	Entity *entity = Entity_Create(game);

	entity->CustomUpdate = DialogBox_Update;
	entity->CustomRender = DialogBox_Render;
	entity->entity_type = ENTITY_DIALOGBOX;
	entity->layer = LAYER_DIALOG;
	entity->properties = malloc(sizeof(DialogBoxProperties));
	
	DialogBoxProperties *properties = (DialogBoxProperties *) entity->properties;

	properties->time_alive = 0;
	properties->current_char = 0;
	properties->show = 0;
	properties->text_index = -1;

	return entity;
}

void DialogBox_Activate(Entity *entity){
	DialogBoxProperties *properties = (DialogBoxProperties *) entity->properties;
	properties->show = 1;
	properties->text_index++;
	properties->current_char = 0;
	properties->time_alive = 0;
}

void DialogBox_Update(Game *game, Entity *entity){
	DialogBoxProperties *properties = (DialogBoxProperties *) entity->properties;
	Entity *player = Entity_Find(game, ENTITY_PLAYER);
	int text_len;

	if(player == NULL) properties->show = 0;
	else if(Player_HasKey(player, KEY_GOLD)) properties->show = 0;
	if(!properties->show){
		properties->time_alive = 0;
		return;
	}

	properties->time_alive += game->delta_time;
	properties->current_char = properties->time_alive / TIME_PER_CHAR;

	if(Input_IsControllerConnected(&game->context))
		text_len = strlen(tutorial_dialog_gamepad[properties->text_index]);
	else
		text_len = strlen(tutorial_dialog[properties->text_index]);

	if(properties->time_alive >= 2 * text_len * TIME_PER_CHAR){
		properties->show = 0;
	}

	if(properties->current_char >= text_len){
		properties->current_char = text_len;
	}
	else if(properties->current_char % 2 == 0){
		Sfx_Play(game->texture_pack->dialog_sfx, 0, 2);
	}
}

void DialogBox_Render(Game *game, Entity *entity){
	DialogBoxProperties *properties = (DialogBoxProperties *) entity->properties;
	char text[256] = "";
	int rect[4] = {0, 3 * game->internal_height / 4, game->internal_width, game->internal_height / 4};
	int bg[4] = {50, 50, 50, 255};
	int fg[4] = {255, 255, 255, 255};

	if(!properties->show) return;

	if(Input_IsControllerConnected(&game->context))
		strncpy(text, tutorial_dialog_gamepad[properties->text_index], properties->current_char);
	else
		strncpy(text, tutorial_dialog[properties->text_index], properties->current_char);

	Draw_DrawRect(&game->context, rect, bg);

	Draw_DrawTextCentered(
			&game->context,
			rect[0] + rect[2] / 2,
			rect[1] + rect[3] / 2,
			fg,
			text
			);
}

Entity * DialogTrigger_Create(Game *game){
	Entity *entity = Entity_Create(game);

	entity->hitbox_size = Vec2_Create(64, 64);
	entity->is_rigid = 0;
	entity->CustomUpdate = DialogTrigger_Update;

	return entity;
}

void DialogTrigger_Update(Game *game, Entity *entity){
	Entity *dialogbox;
	Entity *player = Entity_Find(game, ENTITY_PLAYER);

	if(player == NULL) return;

	if(Entity_CheckCollision(player, entity)){
		entity->is_dead = 1;

		dialogbox = Entity_Find(game, ENTITY_DIALOGBOX);

		if(dialogbox == NULL) return;

		DialogBox_Activate(dialogbox);
	}
}
