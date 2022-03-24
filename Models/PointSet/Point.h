// Define a point of the dataset
#ifndef POINT_H_INCLUDED
#define POINT_H_INCLUDED

#include <array>

class Point {
	private:
		int dimension;
		float* features;
		bool value;
	
	public:
		Point(int dimension, float* features, bool value);
		~Point();
		
		int get_dimension();
		bool get_value();
		float get_feature(std::size_t position);
		float operator[](std::size_t position);
		
};

#endif // POINT_H_INCLUDED
