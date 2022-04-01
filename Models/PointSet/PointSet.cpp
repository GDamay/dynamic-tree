#include "PointSet.h"
#include <math.h>
#include <cstring>
#include <vector>
#include <algorithm> // std::sort

PointSet::PointSet(std::multiset<Point*> points, size_t dimension) : points(points)
{
		this->is_gini_calculated = false;
		this->is_gain_calculated = false;
		this->is_positive_proportion_calculated = false;
		this->dimension = dimension;
}
PointSet::PointSet(const PointSet& source) : points(source.points), dimension(source.dimension)
{
	this->is_positive_proportion_calculated = source.is_positive_proportion_calculated;
	this->is_gini_calculated = source.is_gini_calculated;
	this->is_gain_calculated = source.is_gain_calculated;
	this->positive_counter = source.positive_counter;
	this->positive_proportion = source.positive_proportion;
	this->gini = source.gini;
	if(source.is_gain_calculated)
	{
		this->best_under_counter = source.best_under_counter;
		this->best_under_positive_counter = source.best_under_positive_counter;
		this->best_over_counter = source.best_over_counter;
		this->best_over_positive_counter = source.best_over_positive_counter;
		this->best_gain = source.best_gain;
		this->best_parameter = source.best_parameter;
		this->best_threshold = source.best_threshold;
	}
	
}
PointSet& PointSet::operator=(const PointSet& source)
{
	this->is_positive_proportion_calculated = source.is_positive_proportion_calculated;
	this->is_gini_calculated = source.is_gini_calculated;
	this->is_gain_calculated = source.is_gain_calculated;
	this->positive_counter = source.positive_counter;
	this->positive_proportion = source.positive_proportion;
	this->gini = source.gini;
	if(source.is_gain_calculated)
	{
		this->best_under_counter = source.best_under_counter;
		this->best_under_positive_counter = source.best_under_positive_counter;
		this->best_over_counter = source.best_over_counter;
		this->best_over_positive_counter = source.best_over_positive_counter;
		this->best_gain = source.best_gain;
		this->best_parameter = source.best_parameter;
		this->best_threshold = source.best_threshold;
	}
	return *this;	
}

PointSet::~PointSet()
{}

unsigned int PointSet::get_size() { return this->points.size();}

float PointSet::get_positive_proportion()
{
	if(!this->is_positive_proportion_calculated)
	{
		this->positive_counter  = 0;
		for(auto it = this->points.begin(); it != this->points.end(); it++)
			this->positive_counter += (*it)->get_value();
		this->positive_proportion = (float)this->positive_counter / (float)this->points.size();
		this-> is_positive_proportion_calculated = true;
	}
	return this->positive_proportion;
}

float PointSet::get_gini()
{
	if(!is_gini_calculated)
	{
		this->gini = (float)2*this->get_positive_proportion()*(1-this->get_positive_proportion());
		this->is_gini_calculated = true;
	}
	return this->gini;
}

void PointSet::calculate_best_gain()
{
	if(!this->is_gain_calculated)
	{
		if(this->points.empty())
		{
			this->best_under_counter = 0;
			this->best_under_positive_counter = 0;
			this->best_over_counter = 0;
			this->best_over_positive_counter = 0;
			this->best_gain = 0;
			this->best_parameter = 0;
			this->best_threshold = 0;
		}
		else
		{
			this->get_positive_proportion(); // To have positive_counter up to date
			std::vector<Point*> points_vector(this->points.begin(), this->points.end());
			unsigned int under_counter;
			unsigned int under_positive_counter;
			unsigned int over_counter;
			unsigned int over_positive_counter;
			float current_param_value;
			float fraction_under, fraction_over;
			float current_gain;
			this->best_gain = NAN;
			this->best_threshold = NAN;
			this->best_parameter = 0;
			for(size_t current_dim = 0; current_dim < this->dimension; current_dim++)
			{
				std::sort(points_vector.begin(), points_vector.end(), [current_dim](Point* const &a, Point* const &b) { return (*a)[current_dim] < (*b)[current_dim]; });
				under_counter = 1;
				under_positive_counter = points_vector[0]->get_value();
				over_counter = this->points.size() - 1;
				over_positive_counter = this->positive_counter - points_vector[0]->get_value();
				for(auto it = points_vector.begin(); it != points_vector.end();)
				{
					current_param_value = (*it)->get_feature(current_dim);

					// At the end of the loop, "it" is on the first point for which the feature is not equal,
					// but the counters don't take that last point into account yet
					for(it++;  it != points_vector.end() && (*it)->get_feature(current_dim) == current_param_value; it++)
					{
						under_counter++;
						under_positive_counter += (*it)->get_value();
						over_counter--;
						over_positive_counter -= (*it)->get_value();
					}
					if(it != points_vector.end())
					{
						fraction_under = under_positive_counter/under_counter;
						fraction_over = over_positive_counter/over_counter;
						current_gain = -(fraction_under*(1-fraction_under) + fraction_over*(1-fraction_over));
						if(isnan(this->best_gain) || current_gain > this->best_gain)
						{
							this->best_under_counter = under_counter;
							this->best_under_positive_counter = under_positive_counter;
							this->best_over_counter = over_counter;
							this->best_over_positive_counter = over_positive_counter;
							this->best_gain = current_gain;
							this->best_parameter = current_dim;
							this->best_threshold = (current_param_value + (*it)->get_feature(current_dim))/2;
						} // If Best param/threshold
						under_counter++;
						under_positive_counter += (*it)->get_value();
						over_counter--;
						over_positive_counter -= (*it)->get_value();
					} // If iterator not at end
				} // For points in vector
			} // For all dimensions
		} // Else (of "if empty set of points")
		this->is_gain_calculated = true;
	} // If not calculated yet
}

size_t PointSet::get_best_index()
{
	this->calculate_best_gain();
	return this->best_gain;
}

float PointSet::get_best_gain()
{
	this->calculate_best_gain();
	return this->gini + (float)2/(float)this->points.size()*this->best_gain;
}

float PointSet::get_best_threshold()
{
	this->calculate_best_gain();
	return this->best_threshold;
}

void PointSet::add_point(Point* new_point)
{
	this->points.insert(new_point);
	if(this->is_positive_proportion_calculated)
	{
		this->positive_counter += new_point->get_value();
		this->positive_proportion = (float)this->positive_counter / (float)this->points.size();
		this->is_gini_calculated = false;
	}
	this->is_gain_calculated = false;
}


void PointSet::delete_point(Point* old_point)
{
	this->points.erase(old_point);
	if(this->is_positive_proportion_calculated)
	{
		this->positive_counter -= old_point->get_value();
		this->positive_proportion = (float)this->positive_counter / (float)this->points.size();
		this->is_gini_calculated = false;
	}
	this->is_gain_calculated = false;
}

std::array<PointSet*, 2> PointSet::split_at_best()
{
	this->calculate_best_gain();
	std::multiset<Point*> points_under;
	std::multiset<Point*> points_over;
	auto it_under = points_under.begin();
	auto it_over = points_over.begin();
	for(auto it = this->points.begin(); it != this->points.end(); it++)
		if((*it)->get_feature(this->best_gain) <= this->best_threshold)
			it_under = points_under.insert(it_under, *it);
		else
			it_over = points_over.insert(it_over, *it);
	std::array<PointSet*, 2> to_return = {new PointSet(points_under, this->dimension), new PointSet(points_over, this->dimension)};
	to_return[0]->positive_proportion = this->best_under_counter == 0 ? 0 :
		(float)this->best_under_positive_counter/(float)this->best_under_counter;
	to_return[0]->is_positive_proportion_calculated = true;
	to_return[1]->positive_proportion = this->best_over_counter == 0 ? 0 :
		(float)this->best_over_positive_counter/(float)this->best_over_counter;
	to_return[1]->is_positive_proportion_calculated = true;
	
	return to_return;
}
