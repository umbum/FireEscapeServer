#include <string>
#include <vector>

#include "controller.h"
#include "sensor.h"

unsigned int Controller::_id = 0;

Controller::Controller(std::string str = "Unknown Controller")
{
	this->id = Controller::_id++;
	setName(str);
}

void Controller::setName(std::string str)
{
	this->name = str;
}

void Controller::addSensor(Sensor s)
{
	sensors.push_back(s);
}

//std::vector<Sensor>& Controller::getSensor()
//{
//	return sensors;
//}
