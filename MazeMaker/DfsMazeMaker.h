#pragma once

#include "IMazeMaker.h"

struct DfsNode {
	int x, y;
	bool visited;

	enum WALL {
		WALL_N,
		WALL_E,
		WALL_S,
		WALL_W,
	};
};

// DfsMazeMaker - Generates a maze based on a stack-based depth-first search algorithm
class DfsMazeMaker : public IMazeMaker
{
public:
	std::unique_ptr<Maze> GenerateMaze(int w, int h);
};

