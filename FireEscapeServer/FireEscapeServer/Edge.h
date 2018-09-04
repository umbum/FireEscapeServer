#pragma once
#include <utility>
#include <string>

class Edge {
private:
	static int _id;
	int weight;
public:
	const int id;
	const int length;
	const std::string name;
	Edge(std::string _name, int _len);
	int getWeight();
};
