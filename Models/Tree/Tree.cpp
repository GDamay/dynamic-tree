#include "Tree.h"
#include "../PointSet/PointSet.h"

Tree::Tree(std::multiset<Point*> list_of_points, size_t dimension, size_t max_height, float epsilon) :
	list_of_points(list_of_points.begin(), list_of_points.end()),
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
	this->list_of_points.insert(new_point);
	this->root->add_point(new_point);
}

void Tree::delete_point(const float* features, bool value)
{
	Point pattern_to_delete(this->dimension, features, value);
	auto it_to_delete = this->list_of_points.find(&pattern_to_delete);
	if(it_to_delete == this->list_of_points.end())
		throw "Error : Point does not exists";
	this->root->delete_point(*it_to_delete);
	delete *it_to_delete;
	this->list_of_points.erase(it_to_delete);
}
		
bool Tree::decision(const float* features)
{
	return this->root->decision(features);
}
