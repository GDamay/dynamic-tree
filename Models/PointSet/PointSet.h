// Define a set of points of the dataset
#ifndef POINTSET_H_INCLUDED
#define POINTSET_H_INCLUDED

#include <set>
#include <array>
#include <vector>
#include "Point.h"

enum class FeatureType {BINARY, CLASSIFIED, REAL};

class PointSet {
	private:
		std::multiset<Point*> points;
		std::vector<FeatureType> features_types;
		std::vector<bool> is_feature_relevent;
		size_t dimension;
		unsigned int positive_counter;
		float positive_proportion;
		bool is_positive_proportion_calculated;
		float gini;
		bool is_gini_calculated;
		//used for keeping track of number of points <= threashold (0 for first version) for each parameter
		int best_under_counter;
		//used for keeping track of number of positive points <= threashold (0 for first version) for each parameter
		int best_under_positive_counter;
		//used for keeping track of number of points > threashold (0 for first version) for each parameter
		int best_over_counter;
		//used for keeping track of number of positive points > threashold (0 for first version) for each parameter
		int best_over_positive_counter;

		// Proxy lineraly dependant on real gain
		double best_gain;

		size_t best_parameter;
		float best_threshold;
		bool is_gain_calculated;

		void calculate_best_gain();
	
	public:
		PointSet(std::multiset<Point*> points, size_t dimension, std::vector<FeatureType> features_types, std::vector<bool> is_feature_relevent);
		PointSet(const PointSet& source);
		PointSet(const PointSet& source, std::multiset<Point*> new_points);
		PointSet& operator=(const PointSet& source);
		~PointSet();

		size_t get_size();
		float get_positive_proportion();
		float get_gini();
		unsigned int get_training_error();

		// Provides true best gain, not proxy
		double get_best_gain();
		size_t get_best_index();
		float get_best_threshold();

		void add_point(Point* new_point);
		void delete_point(Point* old_point);
		
		//Gives ownership
		std::array<PointSet*, 2> split_at_best();
		std::array<std::multiset<Point*>, 2> split_at_best_multiset();
};
#endif // POINTSET_H_INCLUDED
