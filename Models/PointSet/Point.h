// Define a point of the dataset
#ifndef POINT_H_INCLUDED
#define POINT_H_INCLUDED

class Point {
	private:
		int dimension;
		float* features;
		bool value;
	
	public:
		Point(int dimension, float* features, bool value);
		~Point();
		
};

#endif // POINT_H_INCLUDED
