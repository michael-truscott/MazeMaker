#pragma once

#include "Vec2f.h"

class Player
{
public:
	Player();

	Vec2f GetViewVector();
	Vec2f GetStrafeVector();

	Vec2f pos;
	float angle;
};

