#include "Edge.h"

int Edge::_id = 0;

Edge::Edge(std::string _name, unsigned int _length): id(_id++), name(_name), length(_length) {
	weight = length;
}

void Edge::updateWeight(int _temp, unsigned int _gas)
{
	temperature = _temp;
	gas = _gas;
	weight = length + temperature + gas;
}


