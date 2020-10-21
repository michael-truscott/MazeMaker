#include "IMazeMaker.h"

//=============================================================================================================
// Maze
//=============================================================================================================

Maze::Maze(int w, int h) :
	m_w(w), m_h(h)
{
	m_blocks = new MazeBlock[w*h];
}

Maze::~Maze()
{
	delete[] m_blocks;
}

MazeBlock Maze::GetBlock(int x, int y)
{
	return m_blocks[y * m_w + x];
}

void Maze::SetBlock(int x, int y, BLOCKTYPE type)
{
	m_blocks[y * m_w + x].Type = type;
}

void Maze::GetPlayerStart(int & x, int & y)
{
	for (int i = 0; i < m_w*m_h; i++) {
		if (m_blocks[i].Type == BL_PLAYERSTART) {
			y = i / m_w;
			x = i % m_w;
			return;
		}
	}
	x = y = -1;
}

//=============================================================================================================
// IMazeMaker
//=============================================================================================================
