#include "PointSet.h"
#include <math.h>
#include <cstring>

PointSet::PointSet(std::vector<Point*> points, size_t dimension) : points(points)
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
	this->positive_proportion = source.positive_proportion;
	this->gini = source.gini;
	if(source.is_gain_calculated)
	{
		this->under_counter = new int[this->dimension];
		this->under_positive_counter = new int[this->dimension];
		this->over_counter = new int[this->dimension];
		this->over_positive_counter = new int[this->dimension];
		this->gini_gain = new float[this->dimension];
		memcpy(this->under_counter, source.under_counter, source.dimension*sizeof(int));
		memcpy(this->under_positive_counter, source.under_positive_counter, source.dimension*sizeof(int));
		memcpy(this->over_counter, source.over_counter, source.dimension*sizeof(int));
		memcpy(this->over_positive_counter, source.over_positive_counter, source.dimension*sizeof(int));
		memcpy(this->gini_gain, source.gini_gain, source.dimension*sizeof(float));
	}
	
}
PointSet& PointSet::operator=(const PointSet& source)
{
	this->is_positive_proportion_calculated = source.is_positive_proportion_calculated;
	this->is_gini_calculated = source.is_gini_calculated;
	this->is_gain_calculated = source.is_gain_calculated;
	this->positive_proportion = source.positive_proportion;
	this->gini = source.gini;
	if(source.is_gain_calculated)
	{
		this->under_counter = new int[this->dimension];
		this->under_positive_counter = new int[this->dimension];
		this->over_counter = new int[this->dimension];
		this->over_positive_counter = new int[this->dimension];
		this->gini_gain = new float[this->dimension];
		memcpy(this->under_counter, source.under_counter, source.dimension*sizeof(int));
		memcpy(this->under_positive_counter, source.under_positive_counter, source.dimension*sizeof(int));
		memcpy(this->over_counter, source.over_counter, source.dimension*sizeof(int));
		memcpy(this->over_positive_counter, source.over_positive_counter, source.dimension*sizeof(int));
		memcpy(this->gini_gain, source.gini_gain, source.dimension*sizeof(float));
	}
	return *this;	
}

PointSet::~PointSet()
{
	if(this->is_gain_calculated)
	{
		delete this->under_counter;
		delete this->under_positive_counter;
		delete this->over_counter;
		delete this->over_positive_counter;
		delete this->gini_gain;
	}
}

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

float* PointSet::get_gini_gain()
{
	if(!is_gain_calculated)
	{
		this->under_counter = new int[this->dimension];
		this->under_positive_counter = new int[this->dimension];
		this->over_counter = new int[this->dimension];
		this->over_positive_counter = new int[this->dimension];
		this->gini_gain = new float[this->dimension];
		if(this->points.empty())
		{
			std::fill_n(under_counter, this->dimension, 0);
			std::fill_n(under_positive_counter, this->dimension, 0);
			std::fill_n(over_counter, this->dimension, 0);
			std::fill_n(over_positive_counter, this->dimension, 0);
			this->best_gain = 0;
			std::fill_n(gini_gain, this->dimension, 0);
		}
		else
		{
			auto it = this->points.begin();
			for(size_t i_dimension = 0; i_dimension < dimension; i_dimension++)
			{
				this->under_counter[i_dimension]=(*it)->get_feature(i_dimension)==0;
				this->under_positive_counter[i_dimension]=(*it)->get_feature(i_dimension)==0 && (*it)->get_value();
				this->over_counter[i_dimension]=(*it)->get_feature(i_dimension)>0;
				this->over_positive_counter[i_dimension]=(*it)->get_feature(i_dimension)>0 && (*it)->get_value();
			}
			for(++it; it != this->points.end(); it++)
			{
				for(size_t i_dimension = 0; i_dimension < dimension; i_dimension++)
				{
					this->under_counter[i_dimension]+=(*it)->get_feature(i_dimension)==0;
					this->under_positive_counter[i_dimension]+=(*it)->get_feature(i_dimension)==0 && (*it)->get_value();
					this->over_counter[i_dimension]+=(*it)->get_feature(i_dimension)>0;
					this->over_positive_counter[i_dimension]+=(*it)->get_feature(i_dimension)>0 && (*it)->get_value();
				}
			}
			this->best_gain = 0;
			for(size_t i_dimension = 0; i_dimension < dimension; i_dimension++)
			{
				float fraction_under = (float)this->under_positive_counter[i_dimension]/(float)this->under_counter[i_dimension];
				float fraction_over = (float)this->over_positive_counter[i_dimension]/(float)this->over_counter[i_dimension];
				this->gini_gain[i_dimension] = -(fraction_under*(1-fraction_under) + fraction_over*(1-fraction_over));
				if(isnan(this->gini_gain[this->best_gain]) || this->gini_gain[i_dimension] > this->gini_gain[this->best_gain])
					this->best_gain = i_dimension;
			}
		}
		this->is_gain_calculated = true;
	}
	return this->gini_gain;
}

size_t PointSet::get_best_index()
{
	this->get_gini_gain();
	return this->best_gain;
}

float PointSet::get_best_gain()
{
	this->get_gini_gain();
	return this->gini + (float)2/(float)this->points.size()*this->gini_gain[this->best_gain];
}

std::array<PointSet*, 2> PointSet::split_at_best()
{
	this->get_gini_gain();
	std::vector<Point*> points_under;
	std::vector<Point*> points_over;
	for(auto it = this->points.begin(); it != this->points.end(); it++)
		if((*it)->get_feature(this->best_gain) == 0)
			points_under.push_back(*it);
		else
			points_over.push_back(*it);
	std::array<PointSet*, 2> to_return = {new PointSet(points_under, this->dimension), new PointSet(points_over, this->dimension)};
	to_return[0]->positive_proportion = this->under_counter[this->best_gain] == 0 ? 0 :
		(float)this->under_positive_counter[this->best_gain]/(float)this->under_counter[this->best_gain];
	to_return[0]->is_positive_proportion_calculated = true;
	to_return[1]->positive_proportion = this->over_counter[this->best_gain] == 0 ? 0 :
		(float)this->over_positive_counter[this->best_gain]/(float)this->over_counter[this->best_gain];
	to_return[1]->is_positive_proportion_calculated = true;
	
	return to_return;
}
