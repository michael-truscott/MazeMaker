#pragma once

#include <SDL.h>
#include <memory>
#include "MazeMaker.h"
#include "Player.h"
#include <cmath>

const float MOVE_SPEED = 5.0f;
const float ROTATE_SPEED = 2.5f;
const float MAX_RAYDEPTH = 64.0f;
const float DEFAULT_FOV = (float)M_PI / 3.0f;
const float DEFAULT_WALLSCALE = 2.0f;

struct InputState {
	bool forward;
	bool back;
	bool strafeL;
	bool strafeR;
	bool rotateL;
	bool rotateR;
};

class MazeDemo
{
public:
	MazeDemo();
	~MazeDemo();

	void Init(int w, int h, bool testMap = false);
	void Update(float dt);
	void Render(SDL_Surface *buffer);

	bool IsFinished() { return m_isFinished; }

private:
	bool CollidedWithMap(Vec2f v);
	InputState m_inputState;

	SDL_Surface *m_bricks;
	MazeMaker *m_mazeMaker;
	Player *m_player;
	bool m_isFinished;
	float m_fov;
	bool m_fisheyeCorrection;
	bool m_showMiniMap;
	float m_wallScaleFactor;
};
