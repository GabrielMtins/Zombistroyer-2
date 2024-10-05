#ifndef ENTITY_H
#define ENTITY_H

#include "vec2.h"
#include "game.h"

struct entity_s{
	Vec2 position;
	Vec2 velocity;
	Texture *texture; /* customizable, default: NULL */

	Vec2 hitbox_size; /* customizable */
	Vec2 hitbox_offset; /* customizable, default: (0, 0) */

	int texture_id; /* customizable, default: -1 */
	int is_rigid; /* customizable, default: 1 */
	int is_dead;
	int health; /* customizable, default: 100 */
	int layer; /* customizable, default: SPRITE_LAYER*/
	int entity_type; /* customizable, default: ENTITY_NONE */
	int light_when_damaged;
	int was_damaged;

	double damage_timer;
	double damage_cooldown; /* customizable, default: 0 */

	void *properties; /* customizable, default: NULL */

	void (*CustomUpdate)(Game *, struct entity_s *); /* customizable */
	void (*CustomRender)(Game *, struct entity_s *); /* customizable */
	void (*CustomCollider)(Game *, struct entity_s *); /* customizable */
	void (*CustomDestructor)(Game *, struct entity_s *); /* customizable */
};

typedef struct entity_s Entity;

Entity * Entity_Create(Game *game);

void Entity_Update(Game *game, Entity *entity);

void Entity_Render(Game *game, Entity *entity);

int Entity_CheckCollision(Entity *e1, Entity *e2);

// Only e1 will move
void Entity_SolveCollision(Entity *e1, Entity *e2);

// Returns 1 if the entity got damaged
int Entity_DoDamage(Entity *entity, int damage);

/* Finds the first entity of its type and returns it
 * Useful when there's just one type of entity in the level */
Entity * Entity_Find(Game *game, int entity_type);

/* Sets the entity with a velocity with modulus velocity and
 * a random direction */
void Entity_RandomDirection(Entity *entity, double velocity);

Vec2 Entity_GetHitboxCenter(Entity *entity);

double Entity_DistSqrBetween(Entity *a, Entity *b);

void Entity_SolveCollisionFor(Game *game, Entity *entity, const int *list);

void Entity_Destroy(Game *game, Entity *entity);

#endif
