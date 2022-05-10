#include "PointSet.h"
#include <math.h>
#include <cstring>
#include <algorithm> // std::sort
#include <map>

PointSet::PointSet(std::multiset<Point*> points, size_t dimension, std::vector<FeatureType> features_types, std::vector<bool> is_feature_relevent) : 
		points(points),
		features_types(features_types),
		is_feature_relevent(is_feature_relevent)
{
	this->is_gini_calculated = false;
	this->is_gain_calculated = false;
	this->is_positive_proportion_calculated = false;
	this->dimension = dimension;
}
PointSet::PointSet(const PointSet& source) : 
		points(source.points),
		dimension(source.dimension),
		features_types(source.features_types),
		is_feature_relevent(source.is_feature_relevent)
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
PointSet::PointSet(const PointSet& source, std::multiset<Point*> new_points):
		points(new_points),
		dimension(source.dimension),
		positive_counter(source.positive_counter),
		positive_proportion(source.positive_proportion),
		is_positive_proportion_calculated(source.is_positive_proportion_calculated),
		gini(source.gini),
		is_gini_calculated(source.is_gini_calculated),
		best_under_counter(source.best_under_counter),
		best_under_positive_counter(source.best_under_positive_counter),
		best_over_counter(source.best_over_counter),
		best_over_positive_counter(source.best_over_positive_counter),
		best_gain(source.best_gain),
		best_parameter(source.best_parameter),
		best_threshold(source.best_threshold),
		is_gain_calculated(source.is_gain_calculated),
		features_types(source.features_types),
		is_feature_relevent(source.is_feature_relevent)
{}

PointSet& PointSet::operator=(const PointSet& source)
{
	this->is_positive_proportion_calculated = source.is_positive_proportion_calculated;
	this->is_gini_calculated = source.is_gini_calculated;
	this->is_gain_calculated = source.is_gain_calculated;
	this->positive_counter = source.positive_counter;
	this->positive_proportion = source.positive_proportion;
	this->gini = source.gini;
	this->features_types = source.features_types;
	this->is_feature_relevent = source.is_feature_relevent;
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

size_t PointSet::get_size() { return this->points.size();}

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

unsigned int PointSet::get_training_error()
{
	unsigned int negative_counter = this->points.size() - this->positive_counter;
	return this->positive_counter > negative_counter ? negative_counter : positive_counter;
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
			std::vector<std::map<float, std::array<unsigned long, 2>>> nb_in_class(this->dimension, std::map<float, std::array<unsigned long, 2>>()); // First value in nb and second is nb of positive
			
			// Populate map
			for(auto it = points_vector.begin(); it != points_vector.end(); it++)
			{
				for(size_t current_dim = 0; current_dim < this->dimension; current_dim++)
				{
					if(this->is_feature_relevent[current_dim] && this->features_types[current_dim] != FeatureType::REAL)
					{
						auto it_nb_in_class = nb_in_class[current_dim].find((*it)->get_feature(current_dim));
						if(it_nb_in_class == nb_in_class[current_dim].end())
						{
							nb_in_class[current_dim][(*it)->get_feature(current_dim)] = {1, (*it)->get_value()};
						}
						else
						{
							it_nb_in_class->second[0]++;
							it_nb_in_class->second[1]+= (*it)->get_value();
						}
					}
				}
			}

			unsigned int under_counter;
			unsigned int under_positive_counter;
			unsigned int over_counter;
			unsigned int over_positive_counter;
			float current_param_value;
			double fraction_under, fraction_over;
			double current_gain;
			this->best_gain = NAN;
			this->best_threshold = NAN;
			this->best_parameter = 0;
			// Find best param/threshold
			for(size_t current_dim = 0; current_dim < this->dimension; current_dim++)
			{
				if(this->features_types[current_dim] == FeatureType::REAL)
				{
					std::sort(points_vector.begin(), points_vector.end(), [current_dim](Point* const &a, Point* const &b) { return (*a)[current_dim] < (*b)[current_dim]; });
					under_counter = 1;
					under_positive_counter = points_vector[0]->get_value();
					over_counter = (unsigned int)this->points.size() - 1;
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
							fraction_under = (double)under_positive_counter/(double)under_counter;
							fraction_over = (double)over_positive_counter/(double)over_counter;
							current_gain = -((double)under_positive_counter*(1-fraction_under) + (double)over_positive_counter*(1-fraction_over));
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
				}
				else if(this->is_feature_relevent[current_dim])
				{
					for(auto class_it = nb_in_class[current_dim].begin(); class_it != nb_in_class[current_dim].end(); class_it++)
					{
						under_counter = this->get_size() - class_it->second[0];
						under_positive_counter = this->positive_counter - class_it->second[1];
						over_counter = class_it->second[0];
						over_positive_counter = class_it->second[1];
						fraction_under = (double)under_positive_counter/(double)under_counter;
						fraction_over = (double)over_positive_counter/(double)over_counter;
						current_gain = -((double)under_positive_counter*(1-fraction_under) + (double)over_positive_counter*(1-fraction_over));
						if(isnan(this->best_gain) || current_gain > this->best_gain)
						{
							this->best_under_counter = under_counter;
							this->best_under_positive_counter = under_positive_counter;
							this->best_over_counter = over_counter;
							this->best_over_positive_counter = over_positive_counter;
							this->best_gain = current_gain;
							this->best_parameter = current_dim;
							this->best_threshold = class_it->first;
						} // If Best param/threshold
					}
				}
			} // For all dimensions
		} // Else (of "if empty set of points")
		this->is_gain_calculated = true;
	} // If not calculated yet
}

size_t PointSet::get_best_index()
{
	this->calculate_best_gain();
	return this->best_parameter;
}

double PointSet::get_best_gain()
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
	auto points_multisets = this->split_at_best_multiset();
	std::vector<bool> is_feature_relevent_under(is_feature_relevent);
	std::vector<bool> is_feature_relevent_over(is_feature_relevent);
	if(this->features_types[this->best_parameter] == FeatureType::BINARY)
	{
		is_feature_relevent_under[this->best_parameter] = false;
		is_feature_relevent_over[this->best_parameter] = false;
	}
	else if(this->features_types[this->best_parameter] == FeatureType::CLASSIFIED)
	{
		is_feature_relevent_over[this->best_parameter] = false;
	}
	std::array<PointSet*, 2> to_return = {
		new PointSet(points_multisets[0], this->dimension, this->features_types, is_feature_relevent_under), 
		new PointSet(points_multisets[1], this->dimension, this->features_types, is_feature_relevent_over)};
	to_return[0]->positive_proportion = this->best_under_counter == 0 ? 0 :
		(float)this->best_under_positive_counter/(float)this->best_under_counter;
	to_return[0]->positive_counter = this->best_under_positive_counter;
	to_return[0]->is_positive_proportion_calculated = true;
	to_return[1]->positive_proportion = this->best_over_counter == 0 ? 0 :
		(float)this->best_over_positive_counter/(float)this->best_over_counter;
	to_return[1]->positive_counter = this->best_over_positive_counter;
	to_return[1]->is_positive_proportion_calculated = true;
	
	return to_return;
}

std::array<std::multiset<Point*>, 2> PointSet::split_at_best_multiset()
{
	std::array<std::multiset<Point*>, 2>  to_return;
	this->calculate_best_gain();
	auto it_under = to_return[0].begin();
	auto it_over = to_return[1].begin();
	if(this->features_types[this->best_parameter] == FeatureType::REAL)
	{
		for(auto it = this->points.begin(); it != this->points.end(); it++)
			if((*it)->get_feature(this->best_parameter) <= this->best_threshold)
				it_under = to_return[0].insert(it_under, *it);
			else
				it_over = to_return[1].insert(it_over, *it);
	}
	else
	{
		for(auto it = this->points.begin(); it != this->points.end(); it++)
			if((*it)->get_feature(this->best_parameter) != this->best_threshold)
				it_under = to_return[0].insert(it_under, *it);
			else
				it_over = to_return[1].insert(it_over, *it);		
	}
	return to_return;
}
