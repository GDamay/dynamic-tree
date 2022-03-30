#include "Vertex.h"

#include <math.h>


Vertex::Vertex(PointSet* pointset, Vertex* parent, unsigned int remaining_high, float epsilon, bool is_root) :
	pointset(pointset),
	parent(parent),
	remaining_high(remaining_high),
	epsilon(epsilon),
	is_root(is_root)
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
			this->under_child = new Vertex(subsets[0], this, remaining_high-1, this->epsilon);
			this->over_child = new Vertex(subsets[1], this, remaining_high-1, this->epsilon);
		}
		this->is_built = true;
		this->updates_since_last_build = 0;
	}
}

unsigned int Vertex::add_point(Point* new_point)
{
	this->pointset->add_point(new_point);
	if(this->is_built && !this->is_leaf)
	{
		this->updates_since_last_build++;
		if(this->updates_since_last_build >= epsilon*this->pointset->get_size())
		{
			this->is_built = false;

			// Casting will truncate. Since the theoritical result is an integer, the calculated result will be very close to an integer.
			// The 0.5 added to the calculated result ensures that is it above the theoritical result and therefore equals to it after truncate
			return (unsigned int)(pow(1.0+epsilon, ceil(log(this->pointset->get_size())/log(1.0+epsilon))) + 0.5);
		}
		else
		{
			unsigned int threshold = (*new_point)[split_parameter] == 0 ? this->under_child->add_point(new_point) : this->over_child->add_point(new_point);
			if(threshold > 0 && this->pointset->get_size() < threshold)
			{
				// TODO Ensure that we can rebuild only when decision called (more efficient) instead of rebuilding right away (stick to the article)
				this->is_built = false;
				return threshold;
			}
			else
				return 0;
		}
	}
}

bool Vertex::decision(const float* features)
{
	this->build();
	if(this->is_leaf)
		return this->pointset->get_positive_proportion() >= 0.5;
	else
		return features[this->split_parameter] == 0 ? this->under_child->decision(features) : this->over_child->decision(features);
}
