#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include "Models/PointSet/Point.h"
#include "Models/PointSet/PointSet.h"
#include "Models/Tree/Vertex.h"
#include "Models/Tree/Tree.h"
constexpr auto DIMENSION = 3;
constexpr float EPSILON = (float)0.2;
constexpr auto MAX_HEIGHT = 5;
using namespace std;

Tree from_file(std::string file_name,
                size_t dimension,
                char delimiter,
                unsigned int label_position,
                float label_true_value,
                std::vector<size_t> add_indices, std::vector<size_t> del_indices,
                std::vector<Point*> &add_points, std::vector<Point> &del_points)
{
    std::multiset<Point*> tree_points;
    auto it_add = add_indices.begin();
    auto it_del = del_indices.begin();
    std::ifstream data_file(file_name);
    std::string current_line;
    if (data_file.is_open()) {
        for(size_t i = 0;data_file; i++)
        {
            data_file >> current_line;
            if(i == *it_add)
            {
                Point* new_point = new Point(current_line, dimension, delimiter, label_position, label_true_value);
                add_points.push_back(new_point);
                it_add++;
            }
            else
            {
                Point* new_point = new Point(current_line, dimension, delimiter, label_position, label_true_value);
                tree_points.insert(new_point);
            }
			if (i == *it_del)
			{
				Point new_point(current_line, dimension, delimiter, label_position, label_true_value);
				del_points.push_back(new_point);
				it_del++;
			}
        }
    }
    else
        throw "Error when oppening the data file";
    data_file.close();
    return Tree(tree_points, dimension, MAX_HEIGHT, EPSILON);
}

int main(int argc, char *argv[])
{
	/*float value[DIMENSION] = {1.0, 0.0, 0.0};
	Point* test_point = new Point(DIMENSION, value, true);
    std::cout << test_point->get_dimension() << test_point->get_value() << test_point->get_feature(2) << (*test_point)[0] << std::endl;
    value[1] = 1.0; value[2] = 1.0;
    Point* test_second = new Point(DIMENSION, value, true);
    value[0] = 0.0;
    Point* test_third = new Point(DIMENSION, value, false);

    std::multiset<Point*> list_of_points;
    list_of_points.insert(test_point);
    list_of_points.insert(test_second);
    list_of_points.insert(test_third);
    PointSet* test_pointset = new PointSet(list_of_points, DIMENSION);
    std::cout << test_pointset->get_gini() << std::endl;
    std::cout << test_pointset->get_best_gain() << std::endl;
    auto test_splited = test_pointset->split_at_best();
    std::cout<< test_splited[0]->get_gini() << " " << test_splited[1]->get_gini()  << std::endl;

    Vertex root(test_pointset, NULL, 5, EPSILON, true);

    value[1] = 0.0; value[2] = 0.0;
    Point challenge_point(DIMENSION, value, false);
    std::cout << root.decision(value) << std::endl;

	Tree test_tree(list_of_points, DIMENSION, MAX_HEIGHT, EPSILON);
	std::cout << test_tree.decision(value) << std::endl;

	test_tree.add_point(value, true);
    std::cout << test_tree.decision(value) << std::endl;

	test_tree.delete_point(value, true);
    std::cout << test_tree.decision(value) << std::endl;

    delete test_splited[0];
    test_splited[0] = NULL;
    delete test_splited[1];
    test_splited[1] = NULL;
    Point bla("1;2;3;4", 3, ';', 0, 1);*/

    std::string file_name;
    size_t dimension;
    char delimiter = ';';
    unsigned int label_position = 0;
    float label_true_value = 1.0;
    char* end;
    // If the file_name is not provided as argument, the user is asked to provide one
    if(argc>=2)
		if(argv[1] == "--help")
		{
			std::cout << "usage : ./dynamic_tree.exe <file_name> <dimension> <label_position> <label_true_value> <delimiter>" << std::endl;
			return 0;
		}
		else
			file_name=argv[1];
    else
    {
        std::cout << "File name : ";
        std::cin >> file_name;
    }
    if(argc>=3)
        dimension=(size_t)std::strtol(argv[2], &end, 10);
    else
    {
        std::cout << "Dimension : ";
        std::cin >> dimension;
    }
    if(argc>=4)
        label_position=(uint)std::strtol(argv[3], &end, 10);
    if(argc>=5)
        label_true_value=std::strtof(argv[4], &end);
    if(argc>=6)
        delimiter=argv[5][0];
    std::vector<Point*> add_points; std::vector<Point> del_points;
    Tree tree_from_file = from_file(file_name,
                dimension,
                delimiter,
                label_position,
                label_true_value,
                std::vector<size_t> {1, 2, 3, 4, 5}, std::vector<size_t> {4, 5, 6, 7, 8},
                add_points, del_points);
    return 0;
}
