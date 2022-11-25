/**
 * @file Vertex.h
 * Implementation of class Vertex
 */
#ifndef VERTEX_H_INCLUDED
#define VERTEX_H_INCLUDED

#include "../PointSet/PointSet.h"
#include "../PointSet/Point.h"
#include <vector>

/**
 * Count the number of calls to the build method
 *
 * This is meant for evaluating performance of the algoritm
 */
extern unsigned int nb_build; 

/**
 * A Vertex of the tree
 *
 * This class represents a vertex of the tree, and hence deals with relations
 * with parents and children vertices.
 */
class Vertex {
	private:
		/**
		 * Indicates whether this vertex is a leaf
		 *
		 * This vertex should have two children iif this is false.
		 */
		bool is_leaf;
		/**
		 * Indicates whether this vertex is the root (top vertex) of the tree
		 *
		 * This vertex should have a parent vertex iif this is false
		 */
		bool is_root;

		/**
		 * Index of the feature along which split is performed
		 *
		 * If {@link #is_leaf this->is_leaf} is false, this parameter is the
		 * index of the feature along which the split is performed in
		 * this->pointset to produce the pointsets of the two children
		 *
		 * @note When {@link #is_leaf this->is_leaf} is true, this parameter is
		 * 	not relevant and could be uninitialized
		 */
		size_t split_parameter;

		/**
		 * Index of the splitting threshold
		 *
		 * If {@link #is_leaf this->is_leaf} is false, this parameter is the
		 * value of the threshold used for splitting this->pointset along the
		 * feature indicated by this->split_parameter to produce the pointsets
		 * of the two children.
		 *
		 * If the related feature is boolean or classified, points should go in
		 * left leg iif their value is equal to this threshold. If the feature
		 * is real, points should go in left leg iif their value is leq than
		 * this threshold
		 *
		 * @note When {@link #is_leaf this->is_leaf} is true, this parameter is
		 * 	not relevant and could be uninitialized
		 */
		float split_threshold;

		/**
		 * Indicate the number of children layers that can still be added
		 *
		 * This corresponds to the parameter h of the algorithm, minus the
		 * number of ancesters of this vertex +1 (to include this vertex).
		 */
		unsigned int remaining_high;

		/**
		 * Keep tracks of the number of updates made since last build
		 *
		 * This parameter is used for deciding when to rebuild
		 */
		unsigned int updates_since_last_build;

		/// Size of the pointset on last build. Used for choosing when to rebuild
		unsigned int size_at_building;

		/**
		 * Parameter of the algorithm, used for deciding when to rebuild
		 *
		 * Higher epsilon value leads to more frequent rebuild and hence more
		 * precise predictions, but also higher computation time. 
		 *
		 * If epsilon=0, the whole tree will be rebuild at each update.
		 */		 
		float epsilon;

		/// Minimal number of points in the pointset to allow splitting
		unsigned int min_split_points;

		/// Minimal gini value of the pointset to allow splitting
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
		 *		1, although the implementation is able to handle different value
		 */
		float epsilon_transmission;

		/**
		 * The parent vertex of this one
		 * 
		 * This is the vertex that created this one, it will be NULL if
		 * {@link #is_root this->is_root} is true.
		 *
		 * @note Vertices are owned by their parents, and the root vertex is
		 *	owned by the tree, hence this pointer is not owned by this vertex
		 */
		Vertex* parent;

		/**
		 * The left leg child vertex of this one
		 *
		 * @note Vertices are owned by their parents, and the root vertex is
		 *	owned by the tree, hence this pointer is owned by this vertex
		 */
		Vertex* under_child;

		/**
		 * The right leg child vertex of this one
		 *
		 * @note Vertices are owned by their parents, and the root vertex is
		 *	owned by the tree, hence this pointer is owned by this vertex
		 */
		Vertex* over_child;

		/// The Pointset containing the points of this vertex
		PointSet* pointset;

		static unsigned int nb_build;

		/**
		 * Enhanced copy constuctor of Vertex
		 *
		 * Copy the parameters of source except its parent and pointset, which
		 * are provided separately. The epsilon and epsilon_transmission are
		 * copied from parent.
		 * 
		 * This function is meant to avoid recomputing the inital tree when
		 * making several tests
		 *
		 * @param source The vertex from which data should be copied
		 * @param parent The parent vertex of the new vertex
		 * @param pointset The pointset of the new vertex
		 *
		 * @warning This is highly misleading, which is why it is private
		 */
		Vertex(const Vertex& source, Vertex* parent, PointSet* pointset);

	public:
		/**
		 * Main constructor of Vertex
		 *
		 * @param pointset The poinset containing the points of this vertex.
		 *	This constructor takes ownership of this pointset.
		 * @param parent The parent vertex of this one. Should be NULL if
		 *  is_root is true. The constructor does not take ownership of the
		 *	parent.
		 * @param remaining_high The number of children layers that can still be
		 * 	added.
		 * 	This corresponds to the parameter h of the algorithm, minus the
		 * 	number of ancesters of this vertex +1 (to include this vertex).
		 * @param epsilon The epsilon parameter of the algorithm. Please note
		 *	that this parameter is splitted in two since there is also the
		 *	epsilon_transmission parameter, which is however expected to be
		 *	equal to epsilon most of the time
		 * @param min_split_points Minimal number of points in the pointset to
		 *	allow splitting
		 * @param min_split_gini Minimal gini value of the pointset to allow
		 *	splitting
		 * @param epsilon_transmission Epsilon value to use when figuring out
		 * 	which vertex to recompute.
		 * 	Epsilon is used twice in the algorithm : once to decide whether the
		 * 	vertex should be recalculated, and a second time when it has been
		 * 	decided that the vertex should be recalculated, which parent of the
		 * 	vertex or the vertex itself should be recalculate (see the paper for
		 * 	more details). This parameter allow to have a different value of
		 * 	epsilon for the second purpose.
		 * 	In most of the cases, this is expected to be equal to
		 *		1, although the implementation is able to deal with a 
		 * 	different value.
		 * @param is_root Indicates whether this vertex is the root (top vertex)
		 * 	of the tree
		 *
		 * @note This automatically triggers building of the Vertex
		 * @warning Ownership of @p pointset is taken, but not ownership 
		 * 	of @p parent
		 */
		Vertex(PointSet* pointset, Vertex* parent, unsigned int remaining_high, float epsilon, unsigned int min_split_points, float min_split_gini, float epsilon_transmission, bool is_root = false);

		/**
		 * Build new root vertex by copying the provided one
		 *
		 * All the data but epsilon and the points are copied from source. The
		 * new points are expected to be a copy of those in source. This
		 * This function is used when making several tests, to avoid expensive
		 * Tree creation
		 *
		 * @param source Root vertex from which to copy
		 * @param epsilon New epsilon value for the tree
		 * @param epsilon_transmission New epsilon_transmission value
		 * @param new_points Copy of the previous points, owned by the new tree
		 */
		Vertex(const Vertex& source, float epsilon, float epsilon_transmission, std::multiset<Point*> new_points);

		/**
		 * Destructor of Vertex
		 *
		 * Free memory of children
		 */
		~Vertex();

		/**
		 * Build the Vertex
		 *
		 * Decides whether the vertex should be a leaf or not and, in the second
		 * case, choose the feature/threshold maximizing the gini gain and split
		 * the pointset to make two children Vertices
		 */
		void build();

		/**
		 * Add a point to the pointset of the vertex and its children
		 *
		 * This also checks that the number of updates defined using epsilon is
		 * not reached. If it is, it will rebuild itself if it is a root,
		 * rebuild its child if the child should be rebuild but not this vertex,
		 * or return the threshold size below which its parents should also
		 * rebuild
		 *
		 * @param new_point The point to add. The Vertex does not take ownership
		 * 	of it.
		 *
		 * @return 0 if it has not to be rebuilt or if it is a tree, else a size
		 *  threshold. Its highest ancester having a pointset size below that
		 * 	threshold should rebuild (it could be itself if its parent has a
		 * 	pointset bigger than the threshold)
		 */
		unsigned int add_point(Point* new_point);

		/**
		 * Delete a point from the pointset of the vertex and its children
		 *
		 * This also checks that the number of updates defined using epsilon is
		 * not reached. If it is, it will rebuild itself if it is a root,
		 * rebuild its child if the child should be rebuild but not this vertex,
		 * or return the threshold size below which its parents should also
		 * rebuild
		 *
		 * @param old_point The point to remove, referenced by adress. The
		 * 	vertex does not take ownership.
		 *
		 * @return 0 if it has not to be rebuilt or if it is a tree, else a size
		 *  threshold. Its highest ancester having a pointset size below that
		 * 	threshold should rebuild (it could be itself if its parent has a
		 * 	pointset bigger than the threshold)
		 * @warning The pointer should point to a point belonging in the 
		 *  PointSet of the vertex. Providing pointer to a point that does not 
		 *  belong in the PointSet of the vertex would result in unexpected 
		 *  behaviour so far, and will raise exception in a later version.
		 */
		unsigned int delete_point(Point* old_point);


		/**
		 * Create a list of string representing the tree from here
		 *
		 * Each string of the list is a line of the representation. The
		 * representation will contain the size and positive proportion of each
		 * vertex and, when the vertex is not a leaf, its splitting feature and
		 * threshold
		 *
		 * @note This is a reccursive method. To have the full tree
		 * representation, this should be called on the root
		 */
		std::vector<std::string> to_string();

		/**
		 * Get the training error
		 *
		 * This is the absolute number of points in the pointset of this vertex
		 * that would not be associated with the right decision if evaluated.
		 */
		unsigned int get_training_error();

		/**
		 * The decision associated with the given features
		 *
		 * @param features The features of the point to evaluate. The ownership
		 * 	of this array is not taken by the method
		 */
		bool decision(const float* features);

		/**
		 * Get the number of time a Vertex#build method has been called
		 *
		 * This is used for evaluating performance of the algorithm
		 */
		static unsigned int get_nb_build() {return Vertex::nb_build;};

		/// Reset the counter given by Vertex#get_nb_build
		static void reset_nb_build() {Vertex::nb_build = 0;};
};
#endif // VERTEX_H_INCLUDED
