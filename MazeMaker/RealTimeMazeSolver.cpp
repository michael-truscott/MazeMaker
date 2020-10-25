#include "RealTimeMazeSolver.h"
#include <algorithm>
#include <iostream>

RealTimeMazeSolver::RealTimeMazeSolver(Maze * maze, Player * player) :
	m_maze(maze),
	m_player(player),
	m_state(ST_INIT),
	m_distToMove(0),
	m_angleToRotate(0),
	m_shouldRestart(false),
	m_restartCountdown(0)
{

}

void RealTimeMazeSolver::Update(float dt)
{
	switch (m_state) {
	case ST_INIT:
	{
		// look east
		m_player->angle = 0;
		m_state = ST_DECIDE;
		break;
	}
	case ST_DECIDE:
	{
		Decide();
		break;
	}
	case ST_ROTATEL:
	{
		float amount = dt * ROTATE_SPEED;
		amount = std::min(amount, m_angleToRotate);
		m_player->angle += amount;
		m_angleToRotate -= amount;
		if (m_angleToRotate <= 0)
		{
			m_distToMove = 1.0f;
			m_state = ST_MOVE;
		}
		break;
	}
	case ST_ROTATER:
	{
		float amount = dt * ROTATE_SPEED;
		amount = std::min(amount, m_angleToRotate);
		m_player->angle -= amount;
		m_angleToRotate -= amount;
		if (m_angleToRotate <= 0)
		{
			m_distToMove = 1.0f;
			m_state = ST_MOVE;
		}
		break;
	}
	case ST_MOVE:
	{
		// TODO: go to ST_FINISHED if within 1 unit of the exit
		float amount = dt * MOVE_SPEED;
		amount = std::min(amount, m_distToMove);
		Vec2f view = m_player->GetViewVector();

		Vec2f nextPos = m_player->pos + (view * amount);
		int endX, endY;
		m_maze->GetEnd(endX, endY);
		if ((int)nextPos.x == endX && (int)nextPos.y == endY) {
			std::cout << "A winner is you!" << std::endl;
			m_restartCountdown = TIME_TIL_RESTART;
			m_state = ST_FINISHED;
			break;
		}

		m_player->pos = m_player->pos + (view * amount);

		m_distToMove -= amount;
		if (m_distToMove <= 0)
		{
			m_state = ST_DECIDE;
		}
		break;
	}
	case ST_FINISHED:
	{
		if (m_restartCountdown <= 0)
			m_shouldRestart = true;
		else
			m_restartCountdown -= dt;
		break;
	}
	}
}

bool RealTimeMazeSolver::ShouldRestart()
{
	return m_shouldRestart;
}

void RealTimeMazeSolver::Decide()
{
	PLAYER_DIR pDir = GetPlayerDir();
	// is there a path to the left?
	auto leftOffset = GetOffset((PLAYER_DIR)((pDir + 1) % 4));
	if (GetOffsetBlock(leftOffset.first, leftOffset.second).Type != BL_SOLID)
	{
		m_angleToRotate = M_PI / 2;
		m_state = ST_ROTATEL;
		return;
	}
	// is there a path ahead?
	auto forwardOffset = GetOffset(pDir);
	if (GetOffsetBlock(forwardOffset.first, forwardOffset.second).Type != BL_SOLID)
	{
		int endX, endY;
		m_maze->GetEnd(endX, endY);
		if (forwardOffset.first == endX && forwardOffset.second == endY) {
			std::cout << "A winner is you!" << std::endl;
			m_restartCountdown = TIME_TIL_RESTART;
			m_state = ST_FINISHED;
		}
		else {
			m_distToMove = 1.0f;
			m_state = ST_MOVE;
		}
		return;
	}
	// is there a path to the right?
	auto dir = (PLAYER_DIR)(pDir - 1);
	if (dir == -1) dir = DIR_SOUTH;
	auto rightOffset = GetOffset(dir);
	if (GetOffsetBlock(rightOffset.first, rightOffset.second).Type != BL_SOLID)
	{
		m_angleToRotate = M_PI / 2;
		m_state = ST_ROTATER;
		return;
	}

	// neither, need to turn around
	m_angleToRotate = M_PI;
	m_state = ST_ROTATEL;
}

RealTimeMazeSolver::PLAYER_DIR RealTimeMazeSolver::GetPlayerDir()
{
	Vec2f view = m_player->GetViewVector();
	float angle = SDL_atan2f(view.x, view.y) - ((float)M_PI / 4);
	if (angle < 0.0f) angle += (float)(2 * M_PI); // Put in the range [0,2*PI)

	if (angle >= 0.0f && angle < M_PI / 2) return DIR_EAST;
	else if (angle >= M_PI / 2 && angle < M_PI) return DIR_NORTH;
	else if (angle >= M_PI && angle < 3 * M_PI / 2) return DIR_WEST;
	else return DIR_SOUTH;
}

std::pair<int, int> RealTimeMazeSolver::GetOffset(PLAYER_DIR d)
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

MazeBlock RealTimeMazeSolver::GetOffsetBlock(int offX, int offY)
{
	int x = (int)m_player->pos.x + offX;
	int y = (int)m_player->pos.y + offY;

	if (x < 0 || x >= m_maze->Width() ||
		y < 0 || y >= m_maze->Height())
		return MazeBlock{ BL_SOLID }; // prevent out of bounds

	return m_maze->GetBlock(x, y);
}
