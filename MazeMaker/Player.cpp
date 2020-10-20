#include "Player.h"
#include <SDL.h>

Player::Player() :
	pos(0,0),
	angle(0)
{
}

Vec2f Player::GetViewVector()
{
	return Vec2f { SDL_cosf(angle), -SDL_sinf(angle) };
}

Vec2f Player::GetStrafeVector()
{
	// 90 degree angle to the right of the view vector
	return Vec2f{ SDL_cosf(angle - (float)M_PI / 2), -SDL_sinf(angle - (float)M_PI / 2) };
}
