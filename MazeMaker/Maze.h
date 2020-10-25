#pragma once

#include <SDL.h>
#include <vector>
#include "Vec2f.h"

enum BLOCKTYPE {
	BL_EMPTY,
	BL_SOLID,
};

struct MazeBlock {
	BLOCKTYPE Type;
};

struct Sprite {
	Vec2f pos;
	SDL_Surface *bitmap;
	float offsetY;
};

class Maze {
public:
	Maze(int w, int h);
	~Maze();

	int Width() { return m_w; }
	int Height() { return m_h; }

	void Clear();
	MazeBlock GetBlock(int x, int y);
	void SetBlock(int x, int y, BLOCKTYPE type);

	void GetPlayerStart(int &x, int &y);
	void SetPlayerStart(int x, int y);

	void GetEnd(int &x, int &y);
	void SetEnd(int x, int y);

	std::vector<Sprite*> &GetSprites() { return m_sprites; }
private:
	int m_w, m_h;
	MazeBlock *m_blocks;
	std::vector<Sprite*> m_sprites;

	Sprite *m_start;
	Sprite *m_end;
};