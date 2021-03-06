#pragma once

#include "Maze.h"
#include "Player.h"
#include <utility>
#include "IMazeSolver.h"

class StepwiseMazeSolver : public IMazeSolver
{
public:
	StepwiseMazeSolver(Maze *maze, Player *player);

	void Update(float dt) override;
private:
	const float STEP_TIME = 0.25f;

	float m_accumTime;
	Maze *m_maze;
	Player *m_player;

	void Decide();

	std::pair<int, int> m_nextNode;
	std::pair<int, int> m_currentNode;

	enum PLAYER_DIR {
		DIR_EAST,
		DIR_NORTH,
		DIR_WEST,
		DIR_SOUTH
	};

	PLAYER_DIR m_dir;
	std::pair<int, int> GetOffset(PLAYER_DIR d);
	MazeBlock GetOffsetBlock(int offX, int offY);

	enum SOLVER_STATE {
		ST_INIT,
		ST_DECIDE,
		ST_ROTATEL,
		ST_ROTATER,
		ST_MOVE,
		ST_FINISHED,
	};

	SOLVER_STATE m_state;
};

