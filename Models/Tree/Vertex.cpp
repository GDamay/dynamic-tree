#include "Vertex.h"

#include <math.h>


Vertex::Vertex(PointSet* pointset, Vertex* parent, unsigned int remaining_high, float epsilon, bool is_root) :
	is_root(is_root),
	remaining_high(remaining_high),
	epsilon(epsilon),
	parent(parent),
	pointset(pointset)
{
	this->is_to_build = true;
	this->is_to_clean = false;
}

Vertex::~Vertex()
{
	delete this->pointset;
	if(this->is_to_clean && !this->is_leaf)
	{
		delete this->under_child;
		delete this->over_child;
	}
}

void Vertex::build()
{
	if(this->is_to_build)
	{
		if(this->is_to_clean)
		{
			delete this->under_child;
			delete this->over_child;
		}
		if(this->remaining_high == 0 || this->pointset->get_gini() == 0 || this->pointset->get_best_gain() <= 0)
		{
			this->is_leaf = true;
		}
		else
		{
			this->is_leaf = false;
			this->split_parameter = this->pointset->get_best_index();
			this->split_threshold = this->pointset->get_best_threshold();
			auto subsets = this->pointset->split_at_best();
			this->under_child = new Vertex(subsets[0], this, remaining_high-1, this->epsilon);
			this->over_child = new Vertex(subsets[1], this, remaining_high-1, this->epsilon);
		}
		this->is_to_build = false;
		this->is_to_clean = true;
		this->updates_since_last_build = 0;
	}
}

unsigned int Vertex::add_point(Point* new_point)
{
	this->pointset->add_point(new_point);
	if(!this->is_to_build && !this->is_leaf)
	{
		this->updates_since_last_build++;
		if(this->updates_since_last_build >= epsilon*this->pointset->get_size())
		{
			this->is_to_build = true;

			// Casting will truncate. Since the theoritical result is an integer, the calculated result will be very close to an integer.
			// The 0.5 added to the calculated result ensures that is it above the theoritical result and therefore equals to it after truncate
			return (unsigned int)(pow(1.0+epsilon, ceil(log(this->pointset->get_size())/log(1.0+epsilon))) + 0.5);
		}
		else
		{
			unsigned int threshold = (*new_point)[split_parameter] <= split_threshold ? this->under_child->add_point(new_point) : this->over_child->add_point(new_point);
			if(threshold > 0 && this->pointset->get_size() < threshold)
			{
				// TODO Ensure that we can rebuild only when decision called (more efficient) instead of rebuilding right away (stick to the article)
				this->is_to_build = true;
				return threshold;
			}
			else
				return 0;
		}
	}
	return 0;
}

unsigned int Vertex::delete_point(Point* old_point)
{
	this->pointset->delete_point(old_point);
	if(!this->is_to_build && !this->is_leaf)
	{
		this->updates_since_last_build++;
		if(this->updates_since_last_build >= epsilon*this->pointset->get_size())
		{
			this->is_to_build = true;

			// Casting will truncate. Since the theoritical result is an integer, the calculated result will be very close to an integer.
			// The 0.5 added to the calculated result ensures that is it above the theoritical result and therefore equals to it after truncate
			return (unsigned int)(pow(1.0+epsilon, ceil(log(this->pointset->get_size())/log(1.0+epsilon))) + 0.5);
		}
		else
		{
			unsigned int threshold = (*old_point)[split_parameter] <= split_threshold ? this->under_child->delete_point(old_point) : this->over_child->delete_point(old_point);
			if(threshold > 0 && this->pointset->get_size() < threshold)
			{
				// TODO Ensure that we can rebuild only when decision called (more efficient) instead of rebuilding right away (stick to the article)
				this->is_to_build = true;
				return threshold;
			}
			else
				return 0;
		}
	}
	return 0;
}

bool Vertex::decision(const float* features)
{
	this->build();
	if(this->is_leaf)
		return this->pointset->get_positive_proportion() >= 0.5;
	else
		return features[this->split_parameter] <= split_threshold ? this->under_child->decision(features) : this->over_child->decision(features);
}
