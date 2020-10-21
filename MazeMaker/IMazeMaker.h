#pragma once

#include <memory>
#include "Maze.h"

class IMazeMaker {
public:
	virtual ~IMazeMaker() {};

	virtual std::unique_ptr<Maze> GenerateMaze(int w, int h) = 0;
};