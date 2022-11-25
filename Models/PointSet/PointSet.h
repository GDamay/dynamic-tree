/**
 * @file PointSet.h
 * Definition of class PointSet
 */
#ifndef POINTSET_H_INCLUDED
#define POINTSET_H_INCLUDED

#include <set>
#include <array>
#include <vector>
#include "Point.h"

/// Possible types of features.
enum class FeatureType {
	/// Binary feature, can be 0 or 1.
	BINARY,
	/// Categorized feature, discrete and without logical order.
	CLASSIFIED,
	/// Real feature, (almost) continuous and logically ordered.
	REAL
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
		 *		vertex is under the "true" leg of the split vertex, since
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
		/**
		 * Main constructor of PointSet
		 * 
		 * Construct a PointSet by providing each of its attributes
		 * 
		 * @param points Pointers to all points to be contained in the PointSet.
		 * 	Does not take ownership of the points, since all points are always
		 * 	owned by the Tree.
		 * @param dimension Dimension of the points features arrays.
		 * 	The dimensions of the points should always be equal to this
		 * @param features_types Lists the types of each feature for the points
		 * @param is_feature_relevent Indicates for each feature if it is still
		 * 	relevant to try and split along it.
		 * 	A feature becomes irrelevent when it is binary and have already
		 * 	serve for splitting, or it is classified, have already been used for
		 * 	splitting, and the current leg it the one in which the feature
		 * 	matches the selected class. 
		 */
		PointSet(std::multiset<Point*> points, size_t dimension, std::vector<FeatureType> features_types, std::vector<bool> is_feature_relevent);

		/**
		 * Copy constructor of PointSet
		 * 
		 * Construct PointSet by copying all parameters of source.
		 * If gain has already been calculated, also copy the related data.
		 * 
		 * @param source PointSet from which data will be copied
		 * @warning The points in the set will not be duplicated, only pointers
		 * 	will
		 */
		PointSet(const PointSet& source);

		/**
		 * Enhanced copy constructor of Pointset
		 * 
		 * This allows to build a PointSet using different multiset. All other
		 * data will be copied from source. It allows to have pointers not
		 * pointing to the same points as source
		 * 
		 * @param source PointSet from which data will be copied
		 * @param new_points New points
		 * @warning It assumes that new_points is a duplicate of the points of
		 * 	source and hence, if gain has already be calculated, it copies the
		 * 	related data. However, if new_points is not a duplicate of the
		 * 	points of the source, this may lead to wrong gain data.
		 * 	This function will be changed in a next version to prevent confusion
		 */
		PointSet(const PointSet& source, std::multiset<Point*> new_points);

		/**
		 * PointSet class assignment operator
		 * 
		 * Copy data of the source object.
		 * If gain has already been calculated, the related data will also be
		 * copied
		 * 
		 * @param source Object to copy the data from
		 * @warning The points in the set will not be duplicated, only pointers
		 * 	will
		 */
		PointSet& operator=(const PointSet& source);

		/**
		 * Destructor of PointSet
		 * 
		 * No pointer is owned, therefore this does nothing
		 */
		~PointSet();

		
		/// Get number of points contained in the PointSet
		size_t get_size();

		/**
		 * Get proportion of points in the PointSet that have positive value
		 * 
		 * @note This is calculated only once, then the result is stored and
		 * 	recalculated only marginaly and when needed
		 */
		float get_positive_proportion();

		/**
		 * Get a proxy of the gini value of the PointSet
		 * 
		 * @note This is calculated only once, then the result is stored and
		 * 	recalculated only marginaly and when needed
		 */
		float get_gini();

		/**
		 * Get the training error
		 * 
		 * This is the number of points in the PointSet that does not have the
		 * same value as the majority
		 */
		unsigned int get_training_error();

		/**
		 * Get the gain expecting for cutting along best feature/threshold
		 * 
		 * If it has not been done before, it triggers the search of best
		 * feature/threshold. However if it has already been calculated and
		 * don't need refresh, it will not be recalculated
		 * 
		 * @note Contrary to attribute @p best_gain of this
		 * 	object, the return value is not a proxy but the exact best gain
		 */
		double get_best_gain();

		/// Get index of feature along which splitting maximizes gain
		size_t get_best_index();
		
		/**
		 * Get the threshold at which splitting
		 * 
		 * This has to be compare to the feature given by
		 * {@link #get_best_index() get_best_index} method. If the feature is
		 * boolean or classified, points should go in left leg iif their value
		 * is equal to this threshold. If the feature is real, points should go
		 * in left leg iif their value is leq than this threshold
		 */ 
		float get_best_threshold();

		/**
		 * Add point to the PointSet
		 * 
		 * @param new_point Point to add.
		 * 	The PointSet does not take ownership of it
		 * 
		 * @note This will update positive_proportion and toggle variable
		 * 	indicating that gini and gain have to be recalculated
		 */
		void add_point(Point* new_point);


		/**
		 * Remove point from the PointSet
		 * 
		 * @param old_point Point to remove.
		 * 	The PointSet does not take ownership of it
		 * 
		 * @note This will update positive_proportion and toggle variable
		 * 	indicating that gini and gain have to be recalculated
		 * @throw std::runtime_error When the point does not belong in the 
		 * 	pointset
		 */
		void delete_point(Point* old_point);

		/**
		 * Get the type of a feature
		 * 
		 * Return whether the feature is boolean, classified or real
		 * 
		 * @param feature The index of the feature of which to return the type
		 */
		FeatureType get_feature_type(size_t feature);


		/**
		 * Create two PointSet by splitting to get best gain
		 * 
		 * Split the points of this PointSet along
		 * {@link #get_best_index() "best feature"} and 
		 * {@link #get_best_threshold() "best threshold"}, and build two
		 * PointSet from the resulting sets of points.
		 * 
		 * @note This does give ownership of the two returned PointSet 
		 */
		std::array<PointSet*, 2> split_at_best();
		/**
		 * Create two multisets of points by splitting to get best gain
		 * 
		 * Split the points of this PointSet along
		 * {@link #get_best_index() "best feature"} and 
		 * {@link #get_best_threshold() "best threshold"}
		 * 
		 * @note Since a PointSet does not own its points, this can not give
		 * 	ownership of the points contained in the multisets 
		 */
		std::array<std::multiset<Point*>, 2> split_at_best_multiset();
};
#endif // POINTSET_H_INCLUDED
