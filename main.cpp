#include <iostream>
#include "Models/PointSet/Point.h"

using namespace std;

int main(int argc, char *argv[])
{
	float value[] = {1.0, 2.0, 3.0};
	Point test_point = Point(3, value, true);
    std::cout << test_point.get_dimension() << test_point.get_value() << test_point.get_feature(2) << test_point[0] << std::endl;
    Point test_second = test_point;
    return 0;
}
