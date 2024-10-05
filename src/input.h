#ifndef INPUT_H
#define INPUT_H

#include "context.h"

enum Button{
	BUTTON_NONE = 0,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_SHOOT,
	BUTTON_WPN_PISTOL,
	BUTTON_WPN_SHOTGUN,
	BUTTON_WPN_SUB,
};

int Input_IsControllerConnected(Context *context);

int Input_IsPressed(Context *context, int button);

/* Please normalize it when using this function */
void Input_LeftAnalogDirection(Context *context, double *x, double *y);

/* Please normalize it when using this function */
void Input_RightAnalogDirection(Context *context, double *x, double *y);

/* Returns the current mouse position normalized */
void Input_GetMousePosition(Context *context, double *x, double *y);

#endif
