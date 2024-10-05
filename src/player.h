#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"

Entity * Player_Create(Game *game);

int Player_CurrentWeaponBullet(Entity *player);

int Player_HasKey(Entity *player, int key_color);

void Player_ReceiveHealth(Entity *player, int health);

void Player_ReceiveKey(Entity *player, int key_color);

void Player_ReceiveBullet(Entity *player, int type);

void Player_Update(Game *game, Entity *player);

void Player_Render(Game *game, Entity *player);

#endif
