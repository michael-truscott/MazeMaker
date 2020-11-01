#pragma once

#include <SDL.h>
#include <memory>
#include "IMazeMaker.h"
#include "Player.h"
#include "IMazeSolver.h"

const float MOVE_SPEED = 5.0f;
const float ROTATE_SPEED = 2.5f;
const float MAX_RAYDEPTH = 64.0f;
const float DEFAULT_FOV = (float)M_PI / 3.0f;
const float DEFAULT_WALLSCALE = 0.5f;
const float DEFAULT_SPRITESCALE = 0.3f;
const float WALL_GROW_TIME = 1.0f;
const float TIME_TIL_RESTART = 1.0f;

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

	void Init(int w, int h);
	void Restart();
	void Update(float dt);
	void Render(SDL_Surface *buffer);

	bool IsFinished() { return m_isFinished; }

private:
	void RenderMaze(SDL_Surface *buffer, float *depthBuffer);
	void RenderSprites(SDL_Surface *buffer, float *depthBuffer);

	bool CollidedWithMap(Vec2f v);
	InputState m_inputState;

	SDL_Surface *m_bricks;

	std::unique_ptr<IMazeMaker> m_mazeMaker;
	std::unique_ptr<Maze> m_maze;
	std::unique_ptr<Player> m_player;
	std::unique_ptr<IMazeSolver> m_mazeSolver;

	Sprite* HitRock(float dt);
	bool HitExit(float dt);
	
	bool m_isFinished;
	float m_fov;
	bool m_fisheyeCorrection;
	bool m_showMiniMap;
	float m_wallScaleFactor;
	float m_spriteScaleFactor;
	float *m_depthBuffer;
	bool m_flipView;

	int m_mazeW, m_mazeH;

	enum DemoState {
		ST_START,
		ST_RUNNING,
		ST_WALLSHRINK,
		ST_WALLGROW,
		ST_FINISHED,
	};
	DemoState m_state;
	float m_stateChangeAfter;
	Sprite *m_rockToDelete;
};
