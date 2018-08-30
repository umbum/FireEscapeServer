#pragma once
#include <string>
#include <vector>

#include "sensor.h"

class Controller {
private:
	unsigned int id;
	std::string name;
public:
	static unsigned int _id;
	std::vector<Sensor> sensors;
	Controller(std::string str);
	void setName(std::string str);
	void addSensor(Sensor s);
	//std::vector<Sensor>& getSensor();
};


