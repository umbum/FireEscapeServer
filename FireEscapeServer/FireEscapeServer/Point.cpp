#include "Point.h"

int Point::_id = 0;

Point::Point(std::string _name): id(_id++), name(_name) {
}

