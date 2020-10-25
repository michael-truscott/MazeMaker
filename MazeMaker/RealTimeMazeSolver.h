#pragma once

#include "IMazeSolver.h"
#include "Maze.h"
#include "Player.h"

class RealTimeMazeSolver : public IMazeSolver
{
public:
	RealTimeMazeSolver(Maze *maze, Player *player);
	void Update(float dt) override;
	bool ShouldRestart() override;

private:
	const float MOVE_SPEED = 5.0f;
	const float ROTATE_SPEED = 2.5f;
	const float TIME_TIL_RESTART = 3.0f;

	Maze *m_maze;
	Player *m_player;
	float m_distToMove;
	float m_angleToRotate;

	float m_restartCountdown;
	bool m_shouldRestart;

	void Decide();

	enum SOLVER_STATE {
		ST_INIT,
		ST_DECIDE, // pick next destination, set up move/rotation amount
		ST_ROTATEL, // rotate left by m_angleToRotate radians, then start moving forward again
		ST_ROTATER, // rotate right by m_angleToRotate radians, then start moving forward again
		ST_MOVE, // move forward by m_distToMove units
		ST_FINISHED,
	};

	SOLVER_STATE m_state;

	enum PLAYER_DIR {
		DIR_EAST,
		DIR_NORTH,
		DIR_WEST,
		DIR_SOUTH
	};

	PLAYER_DIR m_dir;

	PLAYER_DIR GetPlayerDir();
	std::pair<int, int> GetOffset(PLAYER_DIR d);
	MazeBlock GetOffsetBlock(int offX, int offY);
};

