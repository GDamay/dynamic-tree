//Define the ground tree class, owning the points
#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include <vector>
#include "Vertex.h"
#include "../PointSet/Point.h",

class Tree {
	private:
		Vertex* root;
		std::vector<Point*> list_of_points;
		size_t dimension;
		size_t max_height;
	public:
		//Takes ownership
		Tree(std::vector<Point*> list_of_points, size_t dimension, size_t max_height);
		~Tree();
		
		void add_point(const float* features, bool value);
		bool decision(const float* features);		
};
#endif // TREE_H_INCLUDED