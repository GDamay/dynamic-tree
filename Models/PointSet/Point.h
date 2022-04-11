// Define a point of the dataset
#ifndef POINT_H_INCLUDED
#define POINT_H_INCLUDED

#include <array>
#include <string>

class Point {
	private:
		size_t dimension;
		float* features;
		bool value;
	
	public:
		Point(size_t dimension, const float* features, const bool value);
		// dimension is the dimension of the features, should be number of parameters + 1
		Point(std::string input, size_t dimension, char delimiter, unsigned int label_position, float label_true_value);
		Point& operator=(const Point& source);
		
		// For  multiset.
		// This is smaller if the first non-equal feature is smaller
		// Or all features are equals and value is smaller 
		bool operator<(const Point& other) const;
		bool operator==(const Point& other) const;
		Point(const Point& source);
		~Point();
		
		size_t get_dimension();
		bool get_value();
		float get_feature(std::size_t position);
		const float* get_features();
		float operator[](std::size_t position);
		
};

#endif // POINT_H_INCLUDED
