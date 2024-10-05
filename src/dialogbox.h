#ifndef DIALOGBOX_H
#define DIALOGBOX_H

#include "entity.h"

Entity * DialogBox_Create(Game *game);

void DialogBox_Activate(Entity *entity);

void DialogBox_Update(Game *game, Entity *entity);

void DialogBox_Render(Game *game, Entity *entity);

Entity * DialogTrigger_Create(Game *game);

void DialogTrigger_Update(Game *game, Entity *entity);

#endif
