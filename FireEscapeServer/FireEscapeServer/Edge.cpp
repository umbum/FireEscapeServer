#include "Edge.h"

int Edge::_id = 0;

Edge::Edge(std::string _name, int _len): id(_id++), name(_name), length(_len) {
	weight = length;
}

int Edge::getWeight()
{
	return weight;
}
