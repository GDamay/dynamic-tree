#include <iostream>
#include <list>
#include "Models/PointSet/Point.h"
#include "Models/PointSet/PointSet.h"
#include "Models/Tree/Vertex.h"
#define DIMENSION 3
using namespace std;

int main(int argc, char *argv[])
{
	float value[DIMENSION] = {1.0, 0.0, 0.0};
	Point test_point = Point(DIMENSION, value, true);
    std::cout << test_point.get_dimension() << test_point.get_value() << test_point.get_feature(2) << test_point[0] << std::endl;
    value[1] = 1.0; value[2] = 1.0;
    Point test_second = Point(DIMENSION, value, true);
    value[0] = 0.0;
    Point test_third = Point(DIMENSION, value, false);

    std::list<Point*> list_of_points;
    list_of_points.push_back(&test_point);
    list_of_points.push_back(&test_second);
    list_of_points.push_back(&test_third);
    PointSet* test_pointset = new PointSet(list_of_points, DIMENSION);
    std::cout << test_pointset->get_gini() << std::endl;
    std::cout << test_pointset->get_best_gain() << std::endl;
    auto test_splited = test_pointset->split_at_best();
    std::cout<< test_splited[0]->get_gini() << " " << test_splited[1]->get_gini()  << std::endl;

    Vertex root(test_pointset, NULL, 5, true);

    value[1] = 0.0; value[2] = 0.0;
    Point challenge_point(DIMENSION, value, false);
    std::cout << root.decision(challenge_point) << std::endl;

    delete test_splited[0];
    delete test_splited[1];
    return 0;
}
