#include "Maze.h"

Maze::Maze(int w, int h) :
	m_w(w), m_h(h),
	m_start(nullptr),
	m_end(nullptr),
	m_sprites()
{
	
	m_blocks = new MazeBlock[w*h];

	m_start = new Sprite{ {1.5f,1.5f}, SDL_LoadBMP("data/start.bmp") };
	Uint32 magenta = SDL_MapRGB(m_start->bitmap->format, 0xFF, 0x00, 0xFF);
	SDL_SetColorKey(m_start->bitmap, SDL_TRUE, magenta);
	m_sprites.push_back(m_start);

	m_end = new Sprite{ {w - 1.5f,h - 1.5f}, SDL_LoadBMP("data/end.bmp") };
	SDL_SetColorKey(m_end->bitmap, SDL_TRUE, magenta);
	m_sprites.push_back(m_end);
}

Maze::~Maze()
{
	delete[] m_blocks;
	// TODO: handle memory less ugly
	for (int i = 0; i < m_sprites.size(); i++) {
		SDL_FreeSurface(m_sprites[i]->bitmap);
		delete m_sprites[i];
	}
	m_sprites.clear();
}

void Maze::Clear()
{
	for (int i = 0; i < m_w * m_h; i++)
		m_blocks[i].Type = BL_SOLID;
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
	x = (int)m_start->pos.x;
	y = (int)m_start->pos.y;
}

void Maze::SetPlayerStart(int x, int y)
{
	m_start->pos.x = x + 0.5;
	m_start->pos.y = y + 0.5;
}

void Maze::GetEnd(int & x, int & y)
{
	x = (int)m_end->pos.x;
	y = (int)m_end->pos.y;
}

void Maze::SetEnd(int x, int y)
{
	m_end->pos.x = x + 0.5f;
	m_end->pos.y = y + 0.5f;
}
