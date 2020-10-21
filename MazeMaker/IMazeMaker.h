#pragma once

#include <memory>

enum BLOCKTYPE {
	BL_EMPTY,
	BL_SOLID,
	BL_PLAYERSTART,
	BL_END,
};

struct MazeBlock {
	BLOCKTYPE Type;
};

class Maze {
public:
	Maze(int w, int h);
	~Maze();

	int Width() { return m_w; }
	int Height() { return m_h; }

	MazeBlock GetBlock(int x, int y);
	void SetBlock(int x, int y, BLOCKTYPE type);

	void GetPlayerStart(int &x, int &y);
private:
	int m_w, m_h;
	MazeBlock *m_blocks;

};

class IMazeMaker {
public:
	virtual ~IMazeMaker() {};

	virtual std::unique_ptr<Maze> GenerateMaze(int w, int h) = 0;
};