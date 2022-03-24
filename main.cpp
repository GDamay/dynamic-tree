#include <iostream>
#include "Models/PointSet/Point.h"

using namespace std;

int main(int argc, char *argv[])
{
	float value[] = {1.0, 2.0, 3.0};
	Point test_point = Point(3, value, true);
    std::cout << "Hello world!" << std::endl;
    return 0;
}
