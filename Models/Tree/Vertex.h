//Represents a vertex of the tree
#ifndef VERTEX_H_INCLUDED
#define VERTEX_H_INCLUDED

#include "../PointSet/PointSet.h"
#include "../PointSet/Point.h"
#include <vector>

extern unsigned int nb_build;

class Vertex {
	private:
		bool is_leaf;
		bool is_root;

		size_t split_parameter;
		float split_threshold;
		unsigned int remaining_high;
		unsigned int updates_since_last_build;
		float epsilon;
		//not owned
		Vertex* parent;
		//owned
		Vertex* under_child;
		//owned
		Vertex* over_child;
		//owned
		PointSet* pointset;

	public:
		//Takes ownership of pointset, not of parent
		Vertex(PointSet* pointset, Vertex* parent, unsigned int remaining_high, float epsilon, bool is_root = false);
		~Vertex();
		void build();
		//Return threshold of size to rebuild, or 0 if no rebuild is needed
		unsigned int add_point(Point* new_point);
		//Return threshold of size to rebuild, or 0 if no rebuild is needed
		unsigned int delete_point(Point* old_point);

		std::vector<std::string> to_string();
		
		//Does not take ownership
		bool decision(const float* features);

		static unsigned int nb_build;
};
#endif // VERTEX_H_INCLUDED
