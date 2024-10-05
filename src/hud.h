#ifndef HUD_H
#define HUD_H

#include "entity.h"

Entity * Hud_Create(Game *game);

void Hud_AddToEnemyCounter(Entity *hud);

void Hud_Update(Game *game, Entity *hud);

void Hud_Render(Game *game, Entity *hud);

#endif
