#include "input.h"

#include "context.h"

static int Input_IsPressedKeyboard(int button);
static int Input_IsPressedGamepad(Context *context, int button);

static int Input_IsPressedKeyboard(int button){
	const uint8_t *keys = SDL_GetKeyboardState(NULL);

	switch(button){
		case BUTTON_LEFT:
			return keys[SDL_SCANCODE_A];
			break;

		case BUTTON_RIGHT:
			return keys[SDL_SCANCODE_D];
			break;

		case BUTTON_UP:
			return keys[SDL_SCANCODE_W];
			break;

		case BUTTON_DOWN:
			return keys[SDL_SCANCODE_S];
			break;

		case BUTTON_SHOOT:
			return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1);
			break;

		case BUTTON_WPN_PISTOL:
			return keys[SDL_SCANCODE_1];
			break;

		case BUTTON_WPN_SHOTGUN:
			return keys[SDL_SCANCODE_2];
			break;

		case BUTTON_WPN_SUB:
			return keys[SDL_SCANCODE_3];
			break;
	}

	return 0;
}

static int Input_IsPressedGamepad(Context *context, int button){
	int dir_x, dir_y;
	int deadzone = 8000;

	dir_x = SDL_GameControllerGetAxis(context->gamepad, SDL_CONTROLLER_AXIS_LEFTX);
	dir_y = SDL_GameControllerGetAxis(context->gamepad, SDL_CONTROLLER_AXIS_LEFTY);

	switch(button){
		case BUTTON_LEFT:
			return dir_x < -deadzone;
			break;

		case BUTTON_RIGHT:
			return dir_x > deadzone;
			break;

		case BUTTON_UP:
			return dir_y < -deadzone;
			break;

		case BUTTON_DOWN:
			return dir_y > deadzone;
			break;

		case BUTTON_SHOOT:
			return
				SDL_GameControllerGetAxis(context->gamepad, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 100
				||
				SDL_GameControllerGetButton(context->gamepad, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
			break;

		case BUTTON_WPN_PISTOL:
			return SDL_GameControllerGetButton(context->gamepad, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
			break;

		case BUTTON_WPN_SHOTGUN:
			return SDL_GameControllerGetButton(context->gamepad, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
			break;

		case BUTTON_WPN_SUB:
			return SDL_GameControllerGetButton(context->gamepad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
			break;
	}

	return 0;
}

int Input_IsControllerConnected(Context *context){
	return context->gamepad != NULL;
}

int Input_IsPressed(Context *context, int button){
	if(context->gamepad == NULL)
		return Input_IsPressedKeyboard(button);
	else
		return Input_IsPressedGamepad(context, button);
}

void Input_LeftAnalogDirection(Context *context, double *x, double *y){
	int dir_x, dir_y;
	int deadzone = 4000;

	dir_x = SDL_GameControllerGetAxis(context->gamepad, SDL_CONTROLLER_AXIS_LEFTX);
	dir_y = SDL_GameControllerGetAxis(context->gamepad, SDL_CONTROLLER_AXIS_LEFTY);

	if(abs(dir_x) < deadzone) dir_x = 0; 
	if(abs(dir_y) < deadzone) dir_y = 0; 

	*x = dir_x;
	*y = dir_y;
}

void Input_RightAnalogDirection(Context *context, double *x, double *y){
	int dir_x, dir_y;
	int deadzone = 4000;

	if(!SDL_GameControllerHasAxis(context->gamepad, SDL_CONTROLLER_AXIS_RIGHTX)){
		dir_x =
			SDL_GameControllerGetButton(context->gamepad, SDL_CONTROLLER_BUTTON_B) -
			SDL_GameControllerGetButton(context->gamepad, SDL_CONTROLLER_BUTTON_X);

		dir_y =
			SDL_GameControllerGetButton(context->gamepad, SDL_CONTROLLER_BUTTON_A) -
			SDL_GameControllerGetButton(context->gamepad, SDL_CONTROLLER_BUTTON_Y);

		*x = 30000 * dir_x;
		*y = 30000 * dir_y;

		return;
	}

	dir_x = SDL_GameControllerGetAxis(context->gamepad, SDL_CONTROLLER_AXIS_RIGHTX);
	dir_y = SDL_GameControllerGetAxis(context->gamepad, SDL_CONTROLLER_AXIS_RIGHTY);

	if(abs(dir_x) < deadzone) dir_x = 0; 
	if(abs(dir_y) < deadzone) dir_y = 0; 

	*x = dir_x;
	*y = dir_y;
}

void Input_GetMousePosition(Context *context, double *x, double *y){
	int x_int, y_int;
	int w, h;

	SDL_GetMouseState(&x_int, &y_int);

	SDL_GetWindowSize(context->window, &w, &h);

	*x = (double) (x_int) / w;
	*y = (double) (y_int) / h;
}
