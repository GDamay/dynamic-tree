#ifndef POINTSET_H_INCLUDED
#define POINTSET_H_INCLUDED

#include <set>
#include <array>
#include <vector>
#include "Point.h"

/// Possible types of features.
enum class FeatureType {
	BINARY, //< Binary feature, can be 0 or 1.
	CLASSIFIED, //< Categorized feature, discrete and without logical order.
	REAL //< Real feature, (almost) continuous and logically ordered.
};

/**
 * A subset of the dataset.\ Contains a collection of Points.
 *
 * This class handles most of the calculations that have to be made on the
 * points, including gini gain and splitting along best feature.
 */
class PointSet {
	private:
		/**
		 * Multiset of the points in the PointSet.
		 *
		 * @note Each point is not owned by the PointSet.
		 * @note Each point dimension shpuld be equal to this->dimension.
		 */
		std::multiset<Point*> points;

		/**
		 * Types of each feature of the dataset points.
		 *
		 * Vector of size "dimension" containing, for each feature, its type.
		 */
		std::vector<FeatureType> features_types;

		/**
		 * Relevance of each feature for gain calculation.
		 *
		 * Vector of size "dimension" containing true for each feature except
		 * the binary/categorial ones that have already been used for splitting
		 * the PointSet of an upper Vertex, and hence will be equal for all
		 * Points.
		 *
		 * @note For categorial features, this will be false only if the
		 *	vertex is under the "true" leg of the split vertex, since
		 * 	on the "false" leg, diversity could still exist between values
		 * 	other than the split one.
		 */
		std::vector<bool> is_feature_relevent;

		/// Dimension of the features array of the points.
		size_t dimension;

		/// Count number of Points which have value equal "true".
		unsigned int positive_counter;

		/// Proportion of Points that have value equal "true".
		float positive_proportion;

		/**
		 * Keep track of calling of get_positive_proportion().
		 *
		 * positive_proportion should be recalculated at each update of the
		 * PointSet. Hence, this boolean is true if positive_proportion should
		 * be recalculated before use.
		 */
		bool is_positive_proportion_calculated;

		/// Gini of this PointSet
		float gini;

		/**
		 * Keep track of calling of get_gini().
		 *
		 * Data related to gini should be recalculated at each update of the
		 * PointSet. Hence, this boolean is true if those data should be
		 * recalculated before use.
		 */
		bool is_gini_calculated;

		/**
		 * Number of points on left leg of the best split.
		 *
		 * If the split is on a real feature, this is the number of points for
		 * whose this feature is under threashold. If the split is on a
		 * binary/categorial feature, this is the number of points for whose
		 * this feature is different from threashold.
		 */
		int best_under_counter;
		/**
		 * Number of positive points on left leg of the best split.
		 *
		 * If the split is on a real feature, this is the number of points with
		 * value equals true for whose this feature is under threashold. If the
		 * split is on a binary/categorial feature, this is the number of
		 * points with value equals true for whose this feature is different
		 * from threashold.
		 */
		int best_under_positive_counter;
		/**
		 * Number of points on right leg of the best split.
		 *
		 * If the split is on a real feature, this is the number of points for
		 * whose this feature is over threashold. If the split is on a
		 * binary/categorial feature, this is the number of points for whose
		 * this feature equals threashold.
		 */
		int best_over_counter;
		/**
		 * Number of positive points on right leg of the best split.
		 *
		 * If the split is on a real feature, this is the number of points with
		 * value equals true for whose this feature is over threashold. If the
		 * split is on a binary/categorial feature, this is the number of
		 * points with value equals true for whose this feature equals
		 * threashold.
		 */
		int best_over_positive_counter;

		/**
		 * Proxy of gain for best feature/threashold.
		 *
		 * Proxy linearly dependant on the gini gain of splitting along
		 * feature/threshold that maximise this gain.
		 */
		double best_gain;

		/// Feature along which splitting maximise gini gain.
		size_t best_parameter;
		/// Threashold in best feature along which splitting maximise gini gain
		float best_threshold;
		/**
		 * Keep track of calling of calculate_best_gain().
		 *
		 * Data related to best gain should be recalculated at each update of
		 * the PointSet. Hence, this boolean is true if those data should be
		 * recalculated before use.
		 */
		bool is_gain_calculated;

		/// Update data related to best gini gain.
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
		FeatureType get_feature_type(size_t feature);
		
		//Gives ownership
		std::array<PointSet*, 2> split_at_best();
		std::array<std::multiset<Point*>, 2> split_at_best_multiset();
};
#endif // POINTSET_H_INCLUDED
