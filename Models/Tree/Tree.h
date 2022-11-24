/**
 * @file Tree.h
 * Definition of class Tree
 */
#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include <set>
#include <vector>
#include "Vertex.h"
#include "../PointSet/Point.h"
#include "../PointSet/PointSet.h"

/**
 * The decision tree
 *
 * This class mainly deals with memory allocation for points
 */
class Tree {
	private:
		/**
		 * Defines the relation between pointers to use multiset
		 *
		 * A pointer is leq to another if the adress is
		 */
		struct point_ptr_compare
		{
			/**
			 * Comparision operator for Point pointers
			 *
			 * A pointer is leq to another if the adress is.
			 *
			 * This is usefull for using multisets of pointers
			 */
			bool operator()(const Point* left, const Point* right) const {return (*left < *right);}
		};

		/// The root vertex of the tree
		Vertex* root;

		/// Mutliset of all the points contained in the tree
		std::multiset<Point*, point_ptr_compare> list_of_points;

		/// Dimension of the features vector of the points
		size_t dimension;

		/// Maximal height of the tree
		size_t max_height;

		/// Epsilon parameter, used for deciding when to rebuild a vertex
		float epsilon;

		/// Min number of points that a Vertex should contain to not be a leaf
		unsigned int min_split_points;

		/// Min value that a Vertex should match to not be a leaf
		float min_split_gini;

		/**
		 * Epsilon value to use when figuring out which vertex to recompute
		 *
		 * Epsilon is used twice in the algorithm : once to decide whether the
		 * vertex should be recalculated, and a second time when it has been
		 * decided that the vertex should be recalculated, which parent of the
		 * vertex or the vertex itself should be recalculate (see the paper for
		 * more details). This parameter allow to have a different value of
		 * epsilon for the second purpose.
		 *
		 * @note In most of the cases, this is expected to be equal to
		 *	Tree#epsilon, although the implementation is able to
		 * 	deal with a different value.
		 */
		float epsilon_transmission;
	public:
		/**
		 * Main constructor of Tree
		 *
		 * @param list_of_points A multiset of the points that should be
		 *	included in the decision tree. The Tree takes ownership of all the
		 *	points.
		 * @param dimension The dimension of the points features vectors
		 * @param max_height The maximal height of the decision tree
		 * @param epsilon The epsilon parameter of the algorithm
		 * @param min_split_points The minimal number of points a Vertex should
		 * 	contain to not be a leaf
		 * @param min_split_gini The minimal gini value a Vertex should match to
		 *	not be a leaf
		 * @param epsilon_transmission Epsilon value to use when figuring out
		 * 	which vertex to recompute.
		 * 	Epsilon is used twice in the algorithm : once to decide whether the
		 * 	vertex should be recalculated, and a second time when it has been
		 * 	decided that the vertex should be recalculated, which parent of the
		 * 	vertex or the vertex itself should be recalculate (see the paper for
		 * 	more details). This parameter allow to have a different value of
		 * 	epsilon for the second purpose.
		 * 	In most of the cases, this is expected to be equal to
		 *	@p epsilon, although the implementation is able to deal with a 
		 * 	different value.
		 * @param features_types A vector of #FeatureType that indicates, for
		 *	each dimension of the features space, if the feature is boolean,
		 *	classified or real.
		 */
		Tree(std::multiset<Point*> list_of_points, size_t dimension, unsigned int max_height, float epsilon, unsigned int min_split_points,	float min_split_gini, float epsilon_transmission, std::vector<FeatureType> features_types);

		/**
		 * Enhanced copy constructor
		 *
		 * When making several tests with only the epsilon value changing, this
		 * is usefull to avoid going through the costly creation process several
		 * times.
		 *
		 * All parameters except epsilon and epsilon_transmission are copied
		 * from source, and the points are copied so that the new Tree has
		 * ownership of the new points
		 *
		 * @param source The tree from which parameters will be copied
		 * @param epsilon The new epsilon value
		 * @param epsilon_transmission The new epsilon_transmission value
		 * @see Tree#Tree(std::multiset<Point*>, size_t, unsigned int, float, unsigned int, float, float, std::vector<FeatureType>)
		 */
		Tree(const Tree& source, float epsilon, float epsilon_transmission);

		/**
		 * Destructor of Tree
		 *
		 * Free memory of root vertex and points
		 */
		~Tree();

		/**
		 * Create string representing the tree
		 *
		 * @see Vertex#to_string
		 */
		std::string to_string();

		/**
		 * Add a point to the decision tree
		 *
		 * A new point is created using the arguments. This may trigger rebuild
		 * of part or all of the Vertices if conditions are matched
		 *
		 * @param features Features of the point.
		 * 	Memory is copy, hence this does not take ownership of the parameter
		 * @param value The decision value of the point
		 * @see Point#Point(size_t, const float*, const bool)
		 */
		void add_point(const float* features, bool value);

		/**
		 * Add an already constructed point to the decision tree This may 
		 * trigger rebuild of part or all of the Vertices if conditions are
		 * matched
		 *
		 * @param to_add The point to add, the Tree takes ownership of it
		 */
		void add_point(Point* to_add);

		/**
		 * Add a point to the Tree by copying it. This may trigger rebuild
		 * of part or all of the Vertices if conditions are matched
		 *
		 * @param to_add Point of which a copy will be added
		 */
		void add_point(Point to_add);

		/**
		 * Delete a point from the decision tree
		 *
		 * Search for the first point of the tree matching the provided features
		 * and value, and delete it from the tree. This may trigger rebuild
		 * of part or all of the Vertices if conditions are matched
		 *
		 * @param features Features of the point to delete. Those are only
		 *	compared to the ones of the points, and hence no ownership is taken
		 * @param value Decision value of the point to delete
		 * @throw std::runtime_error When no point matching criteria has been
		 *	found
		 */
		void delete_point(const float* features, bool value);

		/**
		 * Delete a point from the decision tree
		 *
		 * Search for the first point of the tree matching the features and
		 * value of the provided point and delete it from the tree. This may 
		 * trigger rebuild of part or all of the Vertices if conditions are
		 * matched
		 *
		 * @param to_delete Point with same features and value as the one to
		 *	remove
		 * @throw std::runtime_error When no point matching criteria has been
		 *	found
		 */
		void delete_point(Point to_delete);

		/**
		 * Get the decision of the tree for given features
		 *
		 * @param features Features for which a decision has to be made. No
		 *	ownership is taken.
		 */
		bool decision(const float* features);

		/**
		 * Get the training error
		 *
		 * This is the absolute number of points in the Tree that would not be 
		 * associated with the right decision if evaluated.
		 */
		unsigned int get_training_error();
};
#endif // TREE_H_INCLUDED
