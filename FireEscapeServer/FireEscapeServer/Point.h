#pragma once
#include <string>

class Point {
private:
	static int _id;
public:
	const int id;
	const std::string name;
	/**************************************************/
	Point(std::string _name = "Unspecified");
};

