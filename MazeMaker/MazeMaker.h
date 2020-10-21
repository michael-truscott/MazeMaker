#pragma once

#include "SpanningTreeFinder.h"
#include "IMazeMaker.h"

class PrimMazeMaker : public IMazeMaker {
public:
	std::unique_ptr<Maze> GenerateMaze(int w, int h) override;

private:
	void DrawPassageBetween(Maze *maze, Node *n1, Node *n2);
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

