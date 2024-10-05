#include "draw.h"
#include <string.h>

void Draw_DrawRect(Context *context, const int *rect, const int *color){
	SDL_Rect sdl_rect = {rect[0], rect[1], rect[2], rect[3]};

	SDL_SetRenderDrawColor(context->renderer, color[0], color[1], color[2], color[3]);

	SDL_RenderFillRect(context->renderer, &sdl_rect);
}

void Draw_DrawText(Context *context, int x, int y, const int *color, const char *text){
	if(strlen(text) == 0) return;

	SDL_Color fg = {color[0], color[1], color[2], color[3]};
	SDL_Rect dst_rect;

	SDL_Surface *surface = TTF_RenderText_Solid(
			context->font,
			text,
			fg
			);

	SDL_Texture *texture = SDL_CreateTextureFromSurface(context->renderer, surface);

	dst_rect.x = x;
	dst_rect.y = y;
	dst_rect.w = surface->w;
	dst_rect.h = surface->h;

	SDL_RenderCopy(context->renderer, texture, NULL, &dst_rect);

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

void Draw_DrawTextCentered(Context *context, int x, int y, const int *color, const char *text){
	int w, h;

	TTF_SizeText(context->font, text, &w, &h);
	
	x -= w/2;
	y -= h/2;

	Draw_DrawText(context, x, y, color, text);
}

void Draw_DrawTextWithBox(Context *context, int x, int y, int border, const int *bg, const int *fg, const char *text){
	int w, h;

	TTF_SizeText(context->font, text, &w, &h);

	w += 2 * border;
	h += 2 * border;

	int rect[4] = {x, y, w, h};

	x += border;
	y += border;

	Draw_DrawRect(context, rect, bg);
	Draw_DrawText(context, x, y, fg, text);
}

void Draw_DrawTextWithBoxCentered(Context *context, int x, int y, int border, const int *bg, const int *fg, const char *text){
	int w, h;
	int og_x = x, og_y = y;

	TTF_SizeText(context->font, text, &w, &h);

	w += 2 * border;
	h += 2 * border;

	x -= w/2;
	y -= h/2;
	
	int rect[4] = {x, y, w, h};

	Draw_DrawRect(context, rect, bg);
	Draw_DrawTextCentered(context, og_x, og_y, fg, text);
}
