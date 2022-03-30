#include "Vertex.h"


Vertex::Vertex(PointSet* pointset, Vertex* parent, unsigned int remaining_high, bool is_root) : pointset(pointset), parent(parent), remaining_high(remaining_high), is_root(is_root)
{
	this->is_built = false;
}

Vertex::~Vertex()
{
	delete this->pointset;
	if(this->is_built && !this->is_leaf)
	{
		delete this->under_child;
		delete this->over_child;
	}
}

void Vertex::build()
{
	if(!this->is_built)
	{
		if(this->remaining_high == 0 || this->pointset->get_gini() == 0 || this->pointset->get_best_gain() <= 0)
		{
			this->is_leaf = true;
		}
		else
		{
			this->is_leaf = false;
			this->split_parameter = this->pointset->get_best_index();
			auto subsets = this->pointset->split_at_best();
			this->under_child = new Vertex(subsets[0], this, remaining_high-1);
			this->over_child = new Vertex(subsets[1], this, remaining_high-1);
		}
		this->is_built = true;
	}
}

void Vertex::add_point(Point* new_point)
{
	this->pointset->add_point(new_point);
	if(!this->is_leaf)
		(*new_point)[split_parameter] == 0 ? this->under_child->add_point(new_point) : this->over_child->add_point(new_point);
}

bool Vertex::decision(const float* features)
{
	this->build();
	if(this->is_leaf)
		return this->pointset->get_positive_proportion() >= 0.5;
	else
		return features[this->split_parameter] == 0 ? this->under_child->decision(features) : this->over_child->decision(features);
}
