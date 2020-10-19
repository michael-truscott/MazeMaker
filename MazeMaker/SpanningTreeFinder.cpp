#include "SpanningTreeFinder.h"
#include <iostream>
#include <queue>

Node::Node(int x, int y) :
	m_edges(0),
	m_x(x),
	m_y(y)
{

}

static Node* GetClosestNode(std::vector<Node*> &nodes, Node* node) {
	Node *result = nullptr;
	float smallestDist = std::numeric_limits<float>::max();
	for (auto n : nodes) {
		float currentDist = node->DistanceTo(*n);
		if (currentDist < smallestDist) {
			smallestDist = currentDist;
			result = n;
		}
	}
	return result;
}

// Takes in a list of isolated nodes (i.e. no links to other nodes) and connects them using Prim's algorithm to form a minimum spanning tree
// This method modifies the edge lists of the original elements.
void SpanningTreeFinder::CreateSpanningTree(std::vector<std::unique_ptr<Node>>& nodes)
{
	// create the queue of unconnected nodes
	std::queue<Node*> unconnected;
	std::vector<Node*> connected;
	for (auto &node : nodes) {
		Node *n = node.get();
		unconnected.push(n);
	}

	connected.push_back(unconnected.front());
	unconnected.pop();

	while (!unconnected.empty()) {
		Node *n = unconnected.front();
		unconnected.pop();

		Node *closest = GetClosestNode(connected, n);
		n->AddEdge(closest);
		closest->AddEdge(n);
		connected.push_back(n);
	}
}
