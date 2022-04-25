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
		unsigned int min_split_points;
		float min_split_gini;
		float epsilon_transmission;
	public:
		//Takes ownership
		Tree(std::multiset<Point*> list_of_points, size_t dimension, unsigned int max_height, float epsilon, unsigned int min_split_points,	float min_split_gini, float epsilon_transmission);

		//Used when making several successive tests for avoiding go through the entire initialisation process several time
		Tree(const Tree& source, float epsilon, float epsilon_transmission);
		~Tree();
		
		std::string to_string();

		void add_point(const float* features, bool value);
		// Warning : Takes ownership !
		void add_point(Point* to_add);
		void add_point(Point to_add);
		void delete_point(const float* features, bool value);
		void delete_point(Point to_delete);
		bool decision(const float* features);		
};
#endif // TREE_H_INCLUDED
