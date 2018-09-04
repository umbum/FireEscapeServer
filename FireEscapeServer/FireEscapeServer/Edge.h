#pragma once
#include <utility>
#include <string>

class Edge {
private:
	static int _id;

	unsigned int weight;
	int temperature;
	unsigned int gas;
public:
	const int id;
	const unsigned int length;
	const std::string name;
	/***************************************************/
	Edge(std::string _name, unsigned int _length);
	void updateWeight(int _temp, unsigned int _gas);
	/**************** inline getter ********************/
	inline unsigned int getWeight() {
		return weight;
	}
	inline int getTemperature() {
		return temperature;
	}
	inline unsigned int getGas() {
		return gas;
	}
};

