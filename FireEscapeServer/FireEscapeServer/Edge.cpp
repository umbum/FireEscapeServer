#include "Edge.h"



Edge::Edge(int len) : length(len) {
	weight = len;
}

int Edge::getWeight()
{
	return weight;
}
