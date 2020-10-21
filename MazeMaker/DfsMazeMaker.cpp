#include "DfsMazeMaker.h"
#include <stack>
#include <unordered_set>
#include <cstdlib>
#include <ctime>

std::unique_ptr<Maze> DfsMazeMaker::GenerateMaze(int w, int h)
{
	std::srand((uint32_t)std::time(nullptr));
	auto maze = std::make_unique<Maze>(w, h);
	maze->Clear();

	// Place nodes at least 1 unit in from the border, and at least 2 units away from each other
	// 
	// e.g.
	// #####
	// #N N#
	// #   #
	// #N N#
	// #####
	int eW = (w - 1) / 2;
	int eH = (h - 1) / 2;
	int numNodes = eW * eH;
	auto getRealXY = [](DfsNode *n, int &x, int &y) {
		x = n->x * 2 + 1;
		y = n->y * 2 + 1;
	};
	DfsNode *nodes = new DfsNode[numNodes];
	for (int i = 0; i < numNodes; i++) {
		nodes[i] = { i % eW, i / eW, 0x00 };
	}
	
	auto getNeighbour = [nodes, eW, eH](DfsNode* n, DfsNode::WALL dir) {
		int nX, nY;
		switch (dir) {
		case DfsNode::WALL_N: nX = n->x; nY = n->y - 1; break;
		case DfsNode::WALL_E: nX = n->x + 1; nY = n->y; break;
		case DfsNode::WALL_S: nX = n->x; nY = n->y + 1; break;
		case DfsNode::WALL_W: nX = n->x - 1; nY = n->y; break;
		}
		if (nX < 0 || nX >= eW || nY < 0 || nY >= eH)
			return (DfsNode*)nullptr;
		DfsNode* result = &nodes[nY * eW + nX];
		return result->visited ? (DfsNode*)nullptr : result;
	};
	// Do dfs
	std::stack<DfsNode*> stack;
	int visited = 0;

	DfsNode *startNode = &nodes[0];
	stack.push(startNode);
	while (!stack.empty()) {
		// find all unvisited neighbours
		DfsNode *node = stack.top();
		node->visited = true;
		int realX, realY;
		getRealXY(node, realX, realY);
		maze->SetBlock(realX, realY, BL_EMPTY);
		std::vector<DfsNode::WALL> neighbours;
		if (getNeighbour(node, DfsNode::WALL_N)) neighbours.push_back(DfsNode::WALL_N);
		if (getNeighbour(node, DfsNode::WALL_E)) neighbours.push_back(DfsNode::WALL_E);
		if (getNeighbour(node, DfsNode::WALL_S)) neighbours.push_back(DfsNode::WALL_S);
		if (getNeighbour(node, DfsNode::WALL_W)) neighbours.push_back(DfsNode::WALL_W);

		// no unvisited neighbours?
		if (neighbours.size() == 0) {
			stack.pop();
			continue;
		}

		// pick a random neighbour
		int index = std::rand() % neighbours.size();
		// dig out the connecting wall
		switch (neighbours[index]) {
		case DfsNode::WALL_N: maze->SetBlock(realX, realY - 1, BL_EMPTY); break;
		case DfsNode::WALL_E:maze->SetBlock(realX + 1, realY, BL_EMPTY); break;
		case DfsNode::WALL_S:maze->SetBlock(realX, realY + 1, BL_EMPTY); break;
		case DfsNode::WALL_W:maze->SetBlock(realX - 1, realY, BL_EMPTY); break;
		}
		auto newNode = getNeighbour(node, neighbours[index]);
		stack.push(newNode);
	}
	
	int x, y;
	getRealXY(&nodes[std::rand() % numNodes], x, y);
	maze->SetBlock(x, y, BL_PLAYERSTART);
	getRealXY(&nodes[std::rand() % numNodes], x, y);
	maze->SetBlock(x, y, BL_END);
	delete[] nodes;
	return maze;
}
