//Define the ground tree class, owning the points
#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include <set>
#include "Vertex.h"
#include "../PointSet/Point.h"

class Tree {
	private:
		Vertex* root;
		struct point_ptr_compare
		{
			bool operator()(const Point* left, const Point* right) const {return (*left < *right);}
		};
		std::multiset<Point*, point_ptr_compare> list_of_points;
		size_t dimension;
		size_t max_height;
		float epsilon;
	public:
		//Takes ownership
		Tree(std::multiset<Point*> list_of_points, size_t dimension, size_t max_height, float epsilon);
		~Tree();
		
		void add_point(const float* features, bool value);
		bool decision(const float* features);		
};
#endif // TREE_H_INCLUDED