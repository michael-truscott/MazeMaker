#pragma once

#include <vector>
#include <memory>

class Node {
public:
	Node(int x, int y);
	virtual ~Node() {};
	virtual float DistanceTo(Node &n) const = 0;

	int NumEdges() const { return (int)m_edges.size(); }
	Node* GetEdge(int i) const { return m_edges[i]; }
	void AddEdge(Node* node) { m_edges.push_back(node); }

	int X() { return m_x; }
	int Y() { return m_y; }
protected:
	int m_x;
	int m_y;
	std::vector<Node*> m_edges;
};

class SpanningTreeFinder
{
public:
	void CreateSpanningTree(std::vector<std::unique_ptr<Node>> &nodes);
};

