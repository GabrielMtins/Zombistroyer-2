#include "game.h"
#include "texture.h"
#include "entity.h"
#include "world.h"
#include "hud.h"

static const char *font_dir = "materials/fonts/default.ttf";
static const int font_size = 8;

static void Game_Update(Game *game);
static void Game_Render(Game *game);

static void Game_Update(Game *game){
	Entity **entity = NULL;

	Hud_Update(game, game->hud);

	for(int i = game->entity_stack->size - 1; i >= 0; i--){
		entity = Stack_Get(game->entity_stack, i);
		Entity_Update(game, *entity);
	}

	for(int i = game->entity_stack->size - 1; i >= 0; i--){
		entity = Stack_Get(game->entity_stack, i);

		if((*entity)->is_dead){
			Entity_Destroy(game, *entity);
			Stack_Remove(game->entity_stack, i);
		}
	}
}

static void Game_Render(Game *game){
	Entity **entity = NULL;

	//SDL_SetRenderDrawColor(game->context.renderer, 100, 100, 100, 255);
	SDL_SetRenderDrawColor(game->context.renderer, 0, 0, 0, 255);
	SDL_RenderClear(game->context.renderer);

	/* clears all layers */
	for(int i = 0; i < NUMBER_LAYERS; i++){
		SDL_SetRenderTarget(game->context.renderer, game->context.layer[i]);
		SDL_SetRenderDrawColor(game->context.renderer, 0, 0, 0, 0);
		SDL_RenderClear(game->context.renderer);
	}

	SDL_SetRenderTarget(game->context.renderer, game->context.layer[0]);

	World_Render(game, game->world);
	Hud_Render(game, game->hud);

	for(size_t i = 0; i < game->entity_stack->size; i++){
		entity = Stack_Get(game->entity_stack, i);
		Entity_Render(game, *entity);
	}

	SDL_SetRenderTarget(game->context.renderer, NULL);

	/* copies all textures to the render */
	for(int i = 0; i < NUMBER_LAYERS; i++){
		SDL_RenderCopy(game->context.renderer, game->context.layer[i], NULL, NULL);
	}

	SDL_RenderPresent(game->context.renderer);
}

TexturePack * TexturePack_Load(Context *context){
	TexturePack *texture_pack = (TexturePack *) malloc(sizeof(TexturePack));

	texture_pack->player_texture = Texture_Load(context, "materials/entity/player.png", 32, 32);
	texture_pack->world_texture = Texture_Load(context, "materials/world/texture.png", 16, 32);
	texture_pack->aim_texture = Texture_Load(context, "materials/entity/crosshair.png", 16, 16);
	texture_pack->shadow_texture = Texture_Load(context, "materials/world/shadow.png", 32, 32);
	texture_pack->player_shadow_texture = Texture_Load(context, "materials/entity/player_shadow.png", 32, 32);
	texture_pack->bullet_texture = Texture_Load(context, "materials/entity/bullet.png", 32, 32);
	texture_pack->enemybullet_texture = Texture_Load(context, "materials/entity/enemy_bullet.png", 32, 32);
	texture_pack->zombie_texture = Texture_Load(context, "materials/entity/zombie.png", 16, 32);
	texture_pack->shooter_texture = Texture_Load(context, "materials/entity/shooter.png", 32, 32);
	texture_pack->fixer_texture = Texture_Load(context, "materials/entity/fixer.png", 64, 64);
	texture_pack->stringer_texture = Texture_Load(context, "materials/entity/stringer.png", 32, 32);
	texture_pack->shotgun_texture = Texture_Load(context, "materials/entity/shotgun.png", 32, 32);
	texture_pack->pickup_texture = Texture_Load(context, "materials/entity/pickup.png", 16, 16);
	texture_pack->door_texture = Texture_Load(context, "materials/entity/door.png", 16, 64);
	texture_pack->door_shadow_texture = Texture_Load(context, "materials/entity/door_shadow.png", 16, 64);
	texture_pack->particle_texture = Texture_Load(context, "materials/entity/particle.png", 6, 6);
	texture_pack->uibutton_texture = Texture_Load(context, "materials/entity/buttons.png", 128, 32);

	texture_pack->mainmenu_music = Sfx_LoadMusic("materials/sfx/mainmenu.wav");
	for(int i = 0; i < 5; i++) texture_pack->level_theme[i] = NULL;

	texture_pack->level_theme[0] = Sfx_LoadMusic("materials/sfx/mainmenu.wav");
	texture_pack->level_theme[1] = Sfx_LoadMusic("materials/sfx/unlikely.wav");
	texture_pack->level_theme[2] = Sfx_LoadMusic("materials/sfx/spacer.wav");
	texture_pack->level_theme[3] = Sfx_LoadMusic("materials/sfx/unlikely.wav");
	texture_pack->level_theme[4] = Sfx_LoadMusic("materials/sfx/spacer.wav");

	texture_pack->collectkey_sfx = Sfx_LoadSfx("materials/sfx/collect_key.wav");
	texture_pack->bullet_sfx = Sfx_LoadSfx("materials/sfx/bullet.wav");
	texture_pack->dooropen_sfx = Sfx_LoadSfx("materials/sfx/door_open.wav");
	texture_pack->enemydamage_sfx = Sfx_LoadSfx("materials/sfx/enemydamage.wav");
	texture_pack->dialog_sfx = Sfx_LoadSfx("materials/sfx/dialog.wav");

	Sfx_SetVolume(texture_pack->collectkey_sfx, 15);
	Sfx_SetVolume(texture_pack->bullet_sfx, 20);
	Sfx_SetVolume(texture_pack->enemydamage_sfx, 25);
	Sfx_SetVolume(texture_pack->dialog_sfx, 5);

	return texture_pack;
}

void TexturePack_Destroy(TexturePack *texture_pack){
	Texture_Destroy(texture_pack->player_texture);
	Texture_Destroy(texture_pack->world_texture);
	Texture_Destroy(texture_pack->aim_texture);
	Texture_Destroy(texture_pack->shadow_texture);
	Texture_Destroy(texture_pack->player_shadow_texture);
	Texture_Destroy(texture_pack->bullet_texture);
	Texture_Destroy(texture_pack->enemybullet_texture);
	Texture_Destroy(texture_pack->zombie_texture);
	Texture_Destroy(texture_pack->shooter_texture);
	Texture_Destroy(texture_pack->fixer_texture);
	Texture_Destroy(texture_pack->stringer_texture);
	Texture_Destroy(texture_pack->shotgun_texture);
	Texture_Destroy(texture_pack->pickup_texture);
	Texture_Destroy(texture_pack->door_texture);
	Texture_Destroy(texture_pack->door_shadow_texture);
	Texture_Destroy(texture_pack->particle_texture);
	Texture_Destroy(texture_pack->uibutton_texture);

	Sfx_Destroy(texture_pack->collectkey_sfx);
	Sfx_Destroy(texture_pack->mainmenu_music);
	Sfx_Destroy(texture_pack->bullet_sfx);
	Sfx_Destroy(texture_pack->dooropen_sfx);
	Sfx_Destroy(texture_pack->enemydamage_sfx);
	Sfx_Destroy(texture_pack->dialog_sfx);

	for(int i = 0; i < 5; i++)
		Sfx_Destroy(texture_pack->level_theme[i]);

	free(texture_pack);
}

Game * Game_Create(void){
	Game *game = (Game *) malloc(sizeof(Game));

	game->internal_width = 416;
	game->internal_height = 240;
	game->current_layer = -1;

	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	Mix_Init(MIX_INIT_MP3);
	
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
		fprintf(stderr, "Failed to init audio\n");
	}

	game->context.window = SDL_CreateWindow(
			"Zombistroyer 2",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			game->internal_width * 2,
			game->internal_height * 2,
			SDL_WINDOW_RESIZABLE
			);

	game->context.renderer = SDL_CreateRenderer(
			game->context.window,
			-1,
			0
			);

	game->context.gamepad = SDL_GameControllerOpen(0);
	if(game->context.gamepad == NULL)
		fprintf(stderr, "Failed to open gamepad\n");

	game->context.font = TTF_OpenFont(font_dir, font_size);

	if(game->context.font == NULL)
		fprintf(stderr, "Failed to load font: %s\n", font_dir);

	SDL_RenderSetLogicalSize(
			game->context.renderer,
			game->internal_width,
			game->internal_height
			);

	SDL_SetRenderDrawBlendMode(game->context.renderer, SDL_BLENDMODE_BLEND);

	SDL_ShowCursor(0);

	for(int i = 0; i < NUMBER_LAYERS; i++){
		game->context.layer[i] = SDL_CreateTexture(
				game->context.renderer,
				SDL_PIXELFORMAT_RGBA8888,
				SDL_TEXTUREACCESS_TARGET,
				game->internal_width,
				game->internal_height
				);

		SDL_SetTextureBlendMode(game->context.layer[i], SDL_BLENDMODE_BLEND);
	}

	Game_SetOpacityLayer(game, LAYER_SHADOW, 128);

	game->quit = 0;

	game->camera = Vec2_Create(0, 0);

	game->entity_stack = Stack_Create(Entity *);

	game->texture_pack = TexturePack_Load(&game->context);

	game->world = (void *) World_Create(game);
	game->hud = (void *) Hud_Create(game);

	return game;
}

void Game_SetDrawLayer(Game *game, int i){
	if(i < 0 || i >= NUMBER_LAYERS) return;
	if(i == game->current_layer) return;

	game->current_layer = i;

	SDL_SetRenderTarget(game->context.renderer, game->context.layer[i]);
}

void Game_SetOpacityLayer(Game *game, int i, uint8_t a){
	SDL_SetTextureAlphaMod(game->context.layer[i], a);
}

void Game_CleanEntityStack(Game *game){
	for(size_t i = 0; i < game->entity_stack->size; i++){
		Entity **tmp = NULL;
		tmp = Stack_Get(game->entity_stack, i);
		Entity_Destroy(game, *tmp);
	}

	Stack_Clean(game->entity_stack);

	//game->entity_stack = Stack_Create(Entity *);
}

void Game_Run(Game *game){
	game->inverted_fps = 1000 / 120;
	int old_time = 0, new_time = 0;
	SDL_Event e;

	while(!game->quit){
		while(SDL_PollEvent(&e)){
			if(e.type == SDL_QUIT) game->quit = 1;
		}

		Game_Update(game);
		Game_Render(game);

		new_time = SDL_GetTicks();

		if(new_time - old_time < game->inverted_fps){
			SDL_Delay(game->inverted_fps - new_time + old_time);
		}

		new_time = SDL_GetTicks();

		game->delta_time = (double) (new_time - old_time) / 1000;

		game->delta_time = fmin(game->delta_time, 1.0 / 60);

		old_time = new_time;
	}
}

void Game_Destroy(Game *game){
	TexturePack_Destroy(game->texture_pack);
	Entity_Destroy(game, game->world);
	Entity_Destroy(game, game->hud);
	Game_CleanEntityStack(game);
	Stack_Destroy(game->entity_stack);

	for(int i = 0; i < NUMBER_LAYERS; i++)
		SDL_DestroyTexture(game->context.layer[i]);

	if(game->context.gamepad != NULL)
		SDL_GameControllerClose(game->context.gamepad);

	SDL_DestroyRenderer(game->context.renderer);
	SDL_DestroyWindow(game->context.window);
	TTF_CloseFont(game->context.font);

	SDL_Quit();
	IMG_Quit();
	TTF_Quit();
	Mix_Quit();

	free(game);
}
