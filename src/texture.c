#include "texture.h"

#define LEVEL_OF_WHITE 200

static void Texture_GetSrcDstRect(Texture *texture, int x, int y, int id, SDL_Rect *src, SDL_Rect *dst);
static SDL_Texture * Texture_GetWhiteTexture(SDL_Renderer *renderer, SDL_Surface *surface);

static void Texture_GetSrcDstRect(Texture *texture, int x, int y, int id, SDL_Rect *src, SDL_Rect *dst){
	SDL_Rect dst_rect = {x, y, texture->cell_width, texture->cell_height};

	if(id == -1){
		dst_rect.w = texture->texture_width;
		dst_rect.h = texture->texture_height;
		*dst = dst_rect;
		
		return;
	}

	int num_tex_x = texture->texture_width / texture->cell_width;

	int id_x = (id % num_tex_x);
	int id_y = (id / num_tex_x);

	SDL_Rect src_rect = {
		id_x * texture->cell_width,
		id_y * texture->cell_height,
		texture->cell_width,
		texture->cell_height
	};

	*src = src_rect;
	*dst = dst_rect;
}

static SDL_Texture * Texture_GetWhiteTexture(SDL_Renderer *renderer, SDL_Surface *surface){
	uint8_t r, g, b, a;
	uint32_t *pixels = surface->pixels;

	for(int i = 0; i < surface->w * surface->h; i++){
		SDL_GetRGBA(pixels[i], surface->format, &r, &g, &b, &a);

		if(a != 0){
			r = g = b = 255;
			a = LEVEL_OF_WHITE;
			pixels[i] = SDL_MapRGBA(surface->format, r, g, b, a);
		}
	}

	return SDL_CreateTextureFromSurface(renderer, surface);
}

Texture * Texture_Load(Context *context, const char *filename, int cell_width, int cell_height){
	Texture *texture;
	SDL_Surface *surface = IMG_Load(filename);

	if(surface == NULL){
		fprintf(stderr, "Failed to load texture: %s\n", filename);
		return NULL;
	}

	texture = (Texture *) malloc(sizeof(Texture));

	texture->texture = SDL_CreateTextureFromSurface(context->renderer, surface);
	texture->texture_light = Texture_GetWhiteTexture(context->renderer, surface);
	SDL_SetTextureBlendMode(texture->texture_light, SDL_BLENDMODE_ADD);

	texture->texture_width = surface->w;
	texture->texture_height = surface->h;

	texture->cell_width = cell_width;
	texture->cell_height = cell_height;

	SDL_FreeSurface(surface);

	return texture;
}

void Texture_RenderCell(Context *context, Texture *texture, int x, int y, int id){
	if(context == NULL || texture == NULL) return;

	SDL_Rect src_rect, dst_rect;

	Texture_GetSrcDstRect(texture, x, y, id, &src_rect, &dst_rect);

	if(id == -1){
		SDL_RenderCopy(context->renderer, texture->texture, NULL, &dst_rect);
		return;
	}

	SDL_RenderCopy(context->renderer, texture->texture, &src_rect, &dst_rect);
}

void Texture_RenderCellLighted(Context *context, Texture *texture, int x, int y, int id){
	Texture_RenderCell(context, texture, x, y, id);

	SDL_Rect src_rect, dst_rect;

	Texture_GetSrcDstRect(texture, x, y, id, &src_rect, &dst_rect);

	if(id == -1){
		SDL_RenderCopy(context->renderer, texture->texture, NULL, &dst_rect);
		return;
	}

	SDL_RenderCopy(context->renderer, texture->texture_light, &src_rect, &dst_rect);
}

void Texture_Destroy(Texture *texture){
	SDL_DestroyTexture(texture->texture);
	SDL_DestroyTexture(texture->texture_light);
	free(texture);
}
