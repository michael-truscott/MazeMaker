#include "MazeMaker.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <unordered_set>

//=============================================================================================================
// MazeNode
//=============================================================================================================

MazeNode::MazeNode(int x, int y) :
	Node(x,y)
{
}

float MazeNode::DistanceTo(Node & n) const
{
	// Manhattan distance
	return (float)(std::abs(m_x - n.X()) + std::abs(m_y - n.Y()));
}

//=============================================================================================================
// MazeMaker
//=============================================================================================================

MazeMaker::MazeMaker(int w, int h) :
	m_w(w),
	m_h(h)
{
	m_blocks = new MazeBlock[w*h]{ BL_SOLID };
}

MazeMaker::~MazeMaker()
{
	delete[] m_blocks;
}

void MazeMaker::ClearMaze()
{
	// Empty all
	for (int i = 0; i < m_w * m_h; i++)
		m_blocks[i].Type = BL_SOLID;
}

void MazeMaker::GenerateMaze(float density)
{
	if (density < 0 || density > 1.0f)
		throw std::exception("density must be between 0.0 and 1.0");

	ClearMaze();
	// force nodes to snap to 2 block grid such that all generated nodes are a multiple of 2 blocks distance from each other
	// gives an effective width/height of e.g. (w - 1) / 2
	// e.g. 9x7 becomes 4x3
	// #########
	// #X X X X#
	// #       #
	// #X X X X#
	// #       #
	// #X X X X#
	// #########
	// mazes should favour odd widths/heights to make the most of the available space
	int effectiveWidth = (m_w - 1) / 2;
	int effectiveHeight = (m_h - 1) / 2;
	int maxNodes = effectiveWidth * effectiveHeight;

	std::srand((uint32_t)std::time(nullptr));
	// generate some randomly placed nodes (relative to the map size)
	std::vector<std::unique_ptr<Node>> nodes;
	int nodesToCreate = (int)(maxNodes * density);
	int misses = 0;
	for (int i = 0; i < nodesToCreate; i++)
	{
		
		std::unique_ptr<Node> node;
		
		do
		{
			int x = (std::rand() % effectiveWidth) * 2 + 1;
			int y = (std::rand() % effectiveHeight) * 2 + 1;
			node = std::make_unique<MazeNode>(x, y);
		} while (std::any_of(nodes.begin(), nodes.end(),
			[&node](auto &n) { return node->DistanceTo(*n.get()) < 2.0f; })); // nodes should not be adjacent to each other on the x or y axis
		
		misses++;
		nodes.push_back(std::move(node)); // christ
	}
	std::cout << "Total misses: " << misses << std::endl;
	// generate a spanning tree
	SpanningTreeFinder tf;
	tf.CreateSpanningTree(nodes);
	
	// draw edge paths
	std::unordered_set<Node*> closed;
	std::queue<Node*> open;
	for (auto &n : nodes)
		open.push(n.get());
	
	while (!open.empty()) {
		Node *n1 = open.front();
		open.pop();
		closed.insert(n1);

		for (int i = 0; i < n1->NumEdges(); i++) {
			Node *n2 = n1->GetEdge(i);

			// skip if we've already done this one
			auto iter = closed.find(n2);
			if (iter != closed.end())
				continue;

			DrawPassageBetween(n1, n2);
		}
	}
	// set a player start and end location
	// pick random node for player start
	Node *start = nodes[std::rand() % nodes.size()].get();
	SetBlock(start->X(), start->Y(), BL_PLAYERSTART);
	// pick furthest node away for end location
	auto iterEnd = std::max_element(nodes.begin(), nodes.end(),
		[&start](auto &lhs, auto &rhs) { return start->DistanceTo(*lhs) < start->DistanceTo(*rhs); });
	Node *end = (*iterEnd).get();
	SetBlock(end->X(), end->Y(), BL_END);

}

MazeBlock MazeMaker::GetBlock(int x, int y)
{
#ifdef _DEBUG
	if (x < 0 || x >= m_w)
		throw std::exception("x index out of range");
	if (y < 0 || y >= m_h)
		throw std::exception("y index out of range");
#endif
	return m_blocks[y*m_w + x];
}

void MazeMaker::SetBlock(int x, int y, BLOCKTYPE type)
{
#ifdef _DEBUG
	if (x < 0 || x >= m_w)
		throw std::exception("x index out of range");
	if (y < 0 || y >= m_h)
		throw std::exception("y index out of range");
#endif
	m_blocks[y*m_w + x].Type = type;
}

// cut out blocks between node edges (randomly choose x-first or y-first path for diagonals)
void MazeMaker::DrawPassageBetween(Node *n1, Node *n2)
{
	BLOCKTYPE block = BL_EMPTY;
	auto getSign = [](int n) {
		if (n < 0) return -1;
		else if (n > 0) return 1;
		else return 0;
	};

	SetBlock(n1->X(), n1->Y(), block);
	SetBlock(n2->X(), n2->Y(), block);

	int xDir = getSign(n2->X() - n1->X()); // -1,0,1 - n2 is left/inline/right of n1
	int yDir = getSign(n2->Y() - n1->Y()); // -1,0,1 - n2 is above/inline/beneath of n1

	if (xDir != 0 && yDir != 0) // diagonal
	{
		bool xFirst = std::rand() % 2 == 0;
		if (xFirst) {
			int y = n1->Y();
			int x;
			for (x = n1->X(); x != n2->X(); x += xDir)
				SetBlock(x, y, block);
			for (; y != n2->Y(); y += yDir)
				SetBlock(x, y, block);
		}
		else { // y first
			int x = n1->X();
			int y;
			for (y = n1->Y(); y != n2->Y(); y += yDir)
				SetBlock(x, y, block);
			for (; x != n2->X(); x += xDir)
				SetBlock(x, y, block);
		}
	}
	else if (xDir != 0) {
		// horizontal line
		int y = n1->Y();
		for (int x = n1->X(); x != n2->X(); x += xDir) {
			SetBlock(x, y, block);
		}
	}
	else if (yDir != 0) {
		// vertical line
		int x = n1->X();
		for (int y = n1->Y(); y != n2->Y(); y += yDir) {
			SetBlock(x, y, block);
		}
	}
}
