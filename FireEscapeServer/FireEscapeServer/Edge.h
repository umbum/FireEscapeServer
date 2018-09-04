#pragma once
#include <utility>

class Edge {
private:
	int weight;
public:
	const int length;
	Edge(int l);
	int getWeight();
};
