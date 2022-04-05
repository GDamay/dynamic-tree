#include "Point.h"

#include <cstring>
#include <stdexcept>

Point::Point(size_t dimension, const float* features, const bool value)
{
	this->dimension = dimension;
	this-> value = value;
	this->features = new float[dimension];
	memcpy(this->features, features, dimension*sizeof(float));
}

Point::Point(std::string input, size_t dimension, char delimiter, unsigned int label_position, float label_true_value)
{
	const char* c_input = input.c_str();
	char* end;
	this->dimension = dimension;
	this->features = new float[dimension];
	float value = std::strtof(c_input, &end);
	for(unsigned int i = 0; (int)i <= dimension; value = std::strtof(c_input, &end), i++)
	{
		if(c_input == end)
			throw std::runtime_error("Wrong feature formatting : to few parameters or unexpected char");
		else if(i == label_position)
			this->value = value == label_true_value;
		else
			this->features[i - (i>label_position)] = value;
		c_input = end + 1;
	}
}

Point& Point::operator=(const Point& source)
{
	this->dimension = source.dimension;
	this-> value = source.value;
	this->features = new float[source.dimension];
	memcpy(this->features, source.features, source.dimension*sizeof(float));
	return *this;
}

bool Point::operator<(const Point& other) const
{
	for(size_t i = 0; i < (size_t)dimension; i++)
		if(this->features[i] < other.features[i])
			return true;
		else if(this->features[i] > other.features[i])
			return false;
	return this->value < other.value;
}

bool Point::operator==(const Point& other) const
{
	for(size_t i = 0; i < (size_t)dimension; i++)
		if(this->features[i] != other.features[i])
			return false;
	return this->value == other.value;
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

size_t Point::get_dimension()
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
