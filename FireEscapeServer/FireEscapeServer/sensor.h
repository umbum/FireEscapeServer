#pragma once
#include <string>
#include <vector>

class Sensor {
private:
	unsigned int id;
	std::string name;
	int celsius;   // IN
	int gas;	   // IN
	int weight;    // OUT
public:
	static unsigned int _id;
	Sensor(std::string str);
	unsigned int getId();
	void setName(std::string str);
	std::string getName();
	void updateWeight(int _celsius, int _gas);
	int getWeight();
	int getLedColor();
};
