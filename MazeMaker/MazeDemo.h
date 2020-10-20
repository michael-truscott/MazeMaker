#pragma once

#include <SDL.h>
#include <memory>
#include "MazeMaker.h"
#include "Player.h"

const float MOVE_SPEED = 5.0f;
const float ROTATE_SPEED = 3.0f;
const float MAX_RAYDEPTH = 64.0f;

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

	MazeMaker *m_mazeMaker;
	Player *m_player;
	bool m_isFinished;
	float m_fov;
	bool m_fisheyeCorrection;
};
