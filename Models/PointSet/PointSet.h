// Define a set of points of the dataset
#ifndef POINTSET_H_INCLUDED
#define POINTSET_H_INCLUDED

#include <set>
#include <array>
#include "Point.h"

class PointSet {
	private:
		std::multiset<Point*> points;
		size_t dimension;
		unsigned int positive_counter;
		float positive_proportion;
		bool is_positive_proportion_calculated;
		float gini;
		bool is_gini_calculated;
		//used for keeping track of number of points <= threashold (0 for first version) for each parameter
		int *under_counter;
		//used for keeping track of number of positive points <= threashold (0 for first version) for each parameter
		int *under_positive_counter;
		//used for keeping track of number of points > threashold (0 for first version) for each parameter
		int *over_counter;
		//used for keeping track of number of positive points > threashold (0 for first version) for each parameter
		int *over_positive_counter;
		float* gini_gain;
		size_t best_gain;
		bool is_gain_calculated;
	
	public:
		PointSet(std::multiset<Point*> points, size_t dimension);
		PointSet(const PointSet& source);
		PointSet& operator=(const PointSet& source);
		~PointSet();

		unsigned int get_size();
		float get_positive_proportion();
		float get_gini();
		
		//Warning : keeps ownership !
		//Does not provide true gain, but proxy linearly dependant
		float* get_gini_gain();
		
		//Does provide true best gain
		float get_best_gain();
		size_t get_best_index();

		void add_point(Point* new_point);
		void delete_point(Point* old_point);
		
		//Gives ownership
		std::array<PointSet*, 2> split_at_best();
};
#endif // POINTSET_H_INCLUDED