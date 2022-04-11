#include "Vertex.h"

#include <math.h>


Vertex::Vertex(PointSet* pointset, Vertex* parent, unsigned int remaining_high, float epsilon, bool is_root) :
	is_root(is_root),
	remaining_high(remaining_high),
	epsilon(epsilon),
	parent(parent),
	pointset(pointset),
	under_child(NULL),
	over_child(NULL)
{
	this->build();
}

Vertex::~Vertex()
{
	delete this->pointset;
	if(!this->under_child)
	{
		delete this->under_child;
		delete this->over_child;
	}
}

void Vertex::build()
{
	if(!this->under_child)
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
	this->updates_since_last_build = 0;
}

unsigned int Vertex::add_point(Point* new_point)
{
	this->pointset->add_point(new_point);
	if(!this->is_leaf)
	{
		this->updates_since_last_build++;
		if(this->updates_since_last_build >= epsilon*this->pointset->get_size())
		{
			if(this->is_root) // If is root, parent can not call rebuild
				this->build();

			// Casting will truncate. Since the theoritical result is an integer, the calculated result will be very close to an integer.
			// The 0.5 added to the calculated result ensures that is it above the theoritical result and therefore equals to it after truncate
			return (unsigned int)(pow(1.0+epsilon, ceil(log((double)this->pointset->get_size())/log(1.0+epsilon))) + 0.5);
		}
		else
		{
			Vertex* to_update = (*new_point)[split_parameter] <= split_threshold ? this->under_child : this->over_child;
			unsigned int threshold = to_update->add_point(new_point);
			if(threshold > 0 && this->pointset->get_size() < threshold)
				if(this->is_root)
					this->build();
				else
					return threshold;
			else
			{
				to_update->build();
				return 0;
			}
		}
	}
	return 0;
}

unsigned int Vertex::delete_point(Point* old_point)
{
	this->pointset->delete_point(old_point);
	if(!this->is_leaf)
	{
		this->updates_since_last_build++;
		if(this->updates_since_last_build >= epsilon*this->pointset->get_size())
		{
			if(this->is_root)
				this->build();

			// Casting will truncate. Since the theoritical result is an integer, the calculated result will be very close to an integer.
			// The 0.5 added to the calculated result ensures that is it above the theoritical result and therefore equals to it after truncate
			return (unsigned int)(pow(1.0+epsilon, ceil(log((double)this->pointset->get_size())/log(1.0+epsilon))) + 0.5);
		}
		else
		{
			Vertex* to_update = (*old_point)[split_parameter] <= split_threshold ? this->under_child : this->over_child;
			unsigned int threshold = to_update->delete_point(old_point);
			if(threshold > 0 && this->pointset->get_size() < threshold)
				if(this->is_root)
					this->build();
				else
					return threshold;
			else
			{
				to_update->build();
				return 0;
			}
		}
	}
	return 0;
}

bool Vertex::decision(const float* features)
{
	if(this->is_leaf)
		return this->pointset->get_positive_proportion() >= 0.5;
	else
		return features[this->split_parameter] <= split_threshold ? this->under_child->decision(features) : this->over_child->decision(features);
}

std::vector<std::string> Vertex::to_string()
{
	if(this->is_leaf)
	{
		std::vector<std::string> to_return{"p=" + std::to_string(this->pointset->get_positive_proportion())+ "\n"};
		return to_return;
	}
	else
	{
		std::string basis = "f=" + std::to_string(this->split_parameter) + ";t=" + std::to_string(this->split_threshold);
		std::vector<std::string> to_return = this->over_child->to_string();
		std::string it_over_str = std::string(basis.length()-1, ' ') + "|  " ;
		auto it_over = to_return.begin();
		*it_over = basis + "--" + *it_over;
		for(++it_over; it_over != to_return.end(); it_over++)
			*it_over = it_over_str + *it_over;
		std::vector<std::string> under_str = this->under_child->to_string();
		auto it_under = under_str.begin();
		to_return.push_back(std::string(basis.length()-1, ' ') + "|--" + *it_under);
		std::string it_under_str = std::string(basis.length()+2, ' ');
		for(++it_under; it_under != under_str.end(); it_under++)
			to_return.push_back(it_under_str + *it_under);
		return to_return;
	}
}
