#include "Tree.h"
#include <numeric>
#include <stdexcept>

Tree::Tree(std::multiset<Point*> list_of_points, size_t dimension, unsigned int max_height, float epsilon, unsigned int min_split_points,	float min_split_gini, float epsilon_transmission, std::vector<FeatureType> features_types):
	list_of_points(list_of_points.begin(), list_of_points.end()),
	dimension(dimension),
	max_height(max_height),
	epsilon(epsilon),
	min_split_points(min_split_points),
	min_split_gini(min_split_gini),
	epsilon_transmission(epsilon_transmission)
{
	std::vector<bool> relevant_features(dimension, true);
	PointSet* first_set = new PointSet(list_of_points, dimension, features_types, relevant_features);
	this->root = new Vertex(first_set, NULL, max_height-1, epsilon, min_split_points, min_split_gini, epsilon_transmission, true);
}

Tree::Tree(const Tree& source, float epsilon, float epsilon_transmission) :
	list_of_points(),
	dimension(source.dimension),
	max_height(source.max_height),
	epsilon(epsilon),
	min_split_points(source.min_split_points),
	min_split_gini(source.min_split_gini),
	epsilon_transmission(source.epsilon_transmission)
{
	for(auto it = source.list_of_points.begin(); it != source.list_of_points.end(); it++)
	{
		Point* new_point = new Point(**it);
		this->list_of_points.insert(new_point);
	}
	this->root = new Vertex(*source.root, epsilon, std::multiset<Point*>(this->list_of_points.begin(), this->list_of_points.end()));
}

Tree::~Tree()
{
	delete this->root;
	for(auto it = this->list_of_points.begin(); it != this->list_of_points.end(); it++)
		delete *it;
	
}

std::string Tree::to_string()
{
	std::vector<std::string> vec_of_res = this->root->to_string();
	return std::accumulate(vec_of_res.begin(), vec_of_res.end(), std::string(""));
}

void Tree::add_point(const float* features, bool value)
{
	Point* new_point = new Point(this->dimension, features, value);
	this->add_point(new_point);
}

void Tree::add_point(Point* to_add)
{
	this->list_of_points.insert(to_add);
	this->root->add_point(to_add);
}

void Tree::add_point(Point to_add)
{
	Point* ptr_to_add = new Point(to_add);
	this->add_point(ptr_to_add);
}

void Tree::delete_point(const float* features, bool value)
{
	Point pattern_to_delete(this->dimension, features, value);
	auto it_to_delete = this->list_of_points.find(&pattern_to_delete);
	if(it_to_delete == this->list_of_points.end())
		throw std::runtime_error("Error : Point does not exists");
	this->root->delete_point(*it_to_delete);
	delete *it_to_delete;
	this->list_of_points.erase(it_to_delete);
}

void Tree::delete_point(Point to_delete)
{
	this->delete_point(to_delete.get_features(), to_delete.get_value());
}
		
bool Tree::decision(const float* features)
{
	return this->root->decision(features);
}

unsigned int Tree::get_training_error()
{
	return this->root->get_training_error();
}
