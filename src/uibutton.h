#ifndef UIBUTTON_H
#define UIBUTTON_H

#include "entity.h"

Entity * UiButton_Create(Game *game, int texture, int *clicked);

void UiButton_Update(Game *game, Entity *entity);

#endif
