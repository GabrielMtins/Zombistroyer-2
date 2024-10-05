#ifndef DRAW_H
#define DRAW_H

#include "context.h"

/* Any color or any rect must be in an array format
 * rect: {x, y, w, h};
 * color: {r, g, b, a} */

void Draw_DrawRect(Context *context, const int *rect, const int *color);
void Draw_DrawText(Context *context, int x, int y, const int *color, const char *text);
void Draw_DrawTextCentered(Context *context, int x, int y, const int *color, const char *text);
void Draw_SmallBorderedText(Context *context, int x, int y, const int *bg, const int *fg, const char *text);
void Draw_DrawTextWithBox(Context *context, int x, int y, int border, const int *bg, const int *fg, const char *text);
void Draw_DrawTextWithBoxCentered(Context *context, int x, int y, int border, const int *bg, const int *fg, const char *text);

#endif
