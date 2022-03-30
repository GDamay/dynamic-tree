#include "Tree.h"
#include "../PointSet/PointSet.h"

Tree::Tree(std::vector<Point*> list_of_points, size_t dimension, size_t max_height, float epsilon) :
	list_of_points(list_of_points),
	dimension(dimension),
	max_height(max_height),
	epsilon(epsilon)
{
	PointSet* first_set = new PointSet(list_of_points, dimension);
	this->root = new Vertex(first_set, NULL, max_height-1, epsilon, true);
}

Tree::~Tree()
{
	delete this->root;
	for(auto it = this->list_of_points.begin(); it != this->list_of_points.end(); it++)
		delete *it;
	
}

void Tree::add_point(const float* features, bool value)
{
	Point* new_point = new Point(this->dimension, features, value);
	this->list_of_points.push_back(new_point);
	this->root->add_point(new_point);
}
		
bool Tree::decision(const float* features)
{
	return this->root->decision(features);
}	