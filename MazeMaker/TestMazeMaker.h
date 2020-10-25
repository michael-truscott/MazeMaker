#pragma once

#include "IMazeMaker.h"

class TestMazeMaker : public IMazeMaker
{
public:
	std::unique_ptr<Maze> GenerateMaze(int w, int h) override;
};

