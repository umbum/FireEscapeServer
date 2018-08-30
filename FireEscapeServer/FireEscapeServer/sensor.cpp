#include <string>
#include <vector>

#include "sensor.h"

unsigned int Sensor::_id = 0;

Sensor::Sensor(std::string str = "Unknown Sensor")
{
	this->id = Sensor::_id++;
	setName(str);
}

unsigned int Sensor::getId()
{
	return id;
}

void Sensor::setName(std::string str)
{
	this->name = str;
}

std::string Sensor::getName()
{
	return name;
}

void Sensor::updateWeight(int _celsius, int _gas)
{
	this->celsius = _celsius;
	this->gas     = _gas;
	
	/** TO-DO **/
	this->weight = celsius + gas;
}

int Sensor::getWeight()
{
	return weight;
}



int Sensor::getLedColor() {
	// weight를 이용해 LedColor 계산 및 반환
	return weight;
}