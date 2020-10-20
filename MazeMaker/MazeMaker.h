#pragma once

#include "SpanningTreeFinder.h"

class MazeNode : public Node {
public:
	MazeNode(int x, int y);
	float DistanceTo(Node &n) const override;
};

enum BLOCKTYPE {
	BL_EMPTY,
	BL_SOLID,
	BL_PLAYERSTART,
	BL_END,
};

struct MazeBlock {
	BLOCKTYPE Type;
};

class MazeMaker
{
public:
	MazeMaker(int _w, int h_);
	~MazeMaker();

	void ClearMaze();
	void GenerateMaze(float density = 0.6f);

	MazeBlock GetBlock(int x, int y);
	void SetBlock(int x, int y, BLOCKTYPE type);

	void GetPlayerStart(int &x, int &y);

	int w, h;
private:
	// TODO: smart pointer?
	MazeBlock* m_blocks;

	void DrawPassageBetween(Node *n1, Node *n2);
};

