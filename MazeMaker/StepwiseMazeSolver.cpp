#include "StepwiseMazeSolver.h"
#include <SDL.h> // M_PI

StepwiseMazeSolver::StepwiseMazeSolver(Maze * maze, Player * player) :
	m_maze(maze),
	m_player(player),
	m_nextNode(0, 0),
	m_currentNode(0,0),
	m_accumTime(0),
	m_state(ST_INIT)
{
}

// Simple "turn left at every intersection" solver
// Stop just in front of the exit
void StepwiseMazeSolver::Update(float dt)
{
	m_accumTime += dt;
	if (m_accumTime < STEP_TIME)
		return;
	m_accumTime -= STEP_TIME;

	auto getAngle = [](PLAYER_DIR d) {
		return (float)(d * M_PI / 2);
	};

	switch (m_state) {
	case ST_INIT:
		// may cause strange results if starting facing a wall
		m_currentNode.first = (int)m_player->pos.x;
		m_currentNode.second = (int)m_player->pos.y;
		m_nextNode.first = (int)m_player->pos.x;
		m_nextNode.second = (int)m_player->pos.y;
		m_dir = DIR_EAST;
		m_player->angle = getAngle(m_dir);
		m_state = ST_DECIDE;
		break;
	case ST_DECIDE:
		Decide();
		Update(STEP_TIME); // Skip rendering the decision steps to make it look nicer
		break;
	case ST_ROTATEL:
	{
		m_dir = (PLAYER_DIR)((m_dir + 1) % 4);
		m_player->angle = getAngle(m_dir);

		auto offset = GetOffset(m_dir);
		if (m_currentNode.first + offset.first == m_nextNode.first &&
			m_currentNode.second + offset.second == m_nextNode.second)
			m_state = ST_MOVE;
		break;
	}
	case ST_ROTATER:
	{
		m_dir = (PLAYER_DIR)(m_dir - 1);
		if (m_dir == -1) m_dir = DIR_SOUTH;

		m_player->angle = getAngle(m_dir);

		auto offset = GetOffset(m_dir);
		if (m_currentNode.first + offset.first == m_nextNode.first &&
			m_currentNode.second + offset.second == m_nextNode.second)
		{
			int endX, endY;
			m_maze->GetEnd(endX, endY);
			if (m_nextNode.first == endX && m_nextNode.second == endY)
				m_state = ST_FINISHED;
			else
				m_state = ST_MOVE;
		}
		break;
	}
	case ST_MOVE:
		m_player->pos.x = m_nextNode.first + 0.5f;
		m_player->pos.y = m_nextNode.second + 0.5f;
		int x, y;
		m_maze->GetEnd(x, y);
		if (m_nextNode.first == x && m_nextNode.second == y)
		{
			m_state = ST_FINISHED;
			break;
		}

		m_state = ST_DECIDE;
		break;
	case ST_FINISHED:
		break;
	}
}

void StepwiseMazeSolver::Decide()
{
	// is there a path to the left?
	auto leftOffset = GetOffset((PLAYER_DIR)((m_dir + 1) % 4));
	if (GetOffsetBlock(leftOffset.first, leftOffset.second).Type != BLOCKTYPE::BL_SOLID)
	{
		m_currentNode = m_nextNode;
		m_nextNode.first += leftOffset.first;
		m_nextNode.second += leftOffset.second;
		m_state = ST_ROTATEL;
		return;
	}
	// is there a path ahead?
	auto forwardOffset = GetOffset(m_dir);
	if (GetOffsetBlock(forwardOffset.first, forwardOffset.second).Type != BLOCKTYPE::BL_SOLID)
	{
		m_currentNode = m_nextNode;
		m_nextNode.first += forwardOffset.first;
		m_nextNode.second += forwardOffset.second;

		int endX, endY;
		m_maze->GetEnd(endX, endY);
		if (m_nextNode.first == endX && m_nextNode.second == endY)
			m_state = ST_FINISHED;
		else
			m_state = ST_MOVE;
		return;
	}
	// is there a path to the right?
	auto dir = (PLAYER_DIR)(m_dir - 1);
	if (dir == -1) dir = DIR_SOUTH;
	auto rightOffset = GetOffset(dir);
	if (GetOffsetBlock(rightOffset.first, rightOffset.second).Type != BLOCKTYPE::BL_SOLID)
	{
		m_currentNode = m_nextNode;
		m_nextNode.first += rightOffset.first;
		m_nextNode.second += rightOffset.second;
		m_state = ST_ROTATER;
		return;
	}


	// neither, need to turn around
	std::swap(m_nextNode, m_currentNode);
	m_state = ST_ROTATEL;
}

std::pair<int, int> StepwiseMazeSolver::GetOffset(PLAYER_DIR d)
{
	int offX, offY;
	switch (d) {
	case DIR_EAST:
		offX = 1; offY = 0;
		break;
	case DIR_NORTH:
		offX = 0; offY = -1;
		break;
	case DIR_WEST:
		offX = -1; offY = 0;
		break;
	case DIR_SOUTH:
		offX = 0; offY = 1;
		break;
	}
	return std::pair<int, int>(offX, offY);
}

MazeBlock StepwiseMazeSolver::GetOffsetBlock(int offX, int offY)
{
	int x = m_nextNode.first + offX;
	int y = m_nextNode.second + offY;

	if (x < 0 || x >= m_maze->Width() ||
		y < 0 || y >= m_maze->Height())
		return MazeBlock{ BLOCKTYPE::BL_SOLID }; // prevent out of bounds

	return m_maze->GetBlock(x, y);
}
