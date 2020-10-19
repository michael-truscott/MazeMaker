#pragma once

#include "Vec2f.h"

class Player
{
public:
	Player();

	Vec2f GetViewVector();
	Vec2f GetStrafeVector();

	float x, y;
	float angle;
};

