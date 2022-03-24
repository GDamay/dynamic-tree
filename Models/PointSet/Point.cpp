#include "Point.h"

#include <cstring>

Point::Point(int dimension, float* features, bool value)
{
	this->dimension = dimension;
	this-> value = value;
	this->features = new float[dimension];
	memcpy(this->features, features, dimension*sizeof(float));
}

Point::~Point()
{
	delete this->features;
}
