#include "Point.h"

#include <cstring>
#include <stdexcept>

Point::Point(const int dimension, const float* features, const bool value)
{
	this->dimension = dimension;
	this-> value = value;
	this->features = new float[dimension];
	memcpy(this->features, features, dimension*sizeof(float));
}

Point& Point::operator=(const Point& source)
{
	this->dimension = source.dimension;
	this-> value = source.value;
	this->features = new float[source.dimension];
	memcpy(this->features, source.features, source.dimension*sizeof(float));
	return *this;
}

Point::Point(const Point& source)
{
	this->dimension = source.dimension;
	this-> value = source.value;
	this->features = new float[source.dimension];
	memcpy(this->features, source.features, source.dimension*sizeof(float));
}

Point::~Point()
{
	delete this->features;
}

int Point::get_dimension()
{
	return this->dimension;
}

bool Point::get_value()
{
	return this-> value;
}

float Point::get_feature(std::size_t position)
{
	if(position < dimension)
		return this->features[position];
	else
		throw std::out_of_range("position should be less than dimension");
}

float Point::operator[](std::size_t position)
{
	return this->get_feature(position);
}
