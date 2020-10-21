#pragma once

#include "SpanningTreeFinder.h"
#include "IMazeMaker.h"

class PrimMazeMaker : public IMazeMaker {
public:
	std::unique_ptr<Maze> GenerateMaze(int w, int h) override;

private:
	void DrawPassageBetween(Maze *maze, Node *n1, Node *n2);
};
