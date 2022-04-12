#include "param_parser.h"
#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <chrono>
#include "Models/PointSet/Point.h"
#include "Models/PointSet/PointSet.h"
#include "Models/Tree/Vertex.h"
#include "Models/Tree/Tree.h"
constexpr auto DIMENSION = 3;
constexpr float EPSILON = (float)0.2;
constexpr auto MAX_HEIGHT = 5;
using namespace std;

enum event_type{ADD, DEL, EVAL};
struct tree_event{
		Point  event_point;
		event_type tree_event_type;
		tree_event( Point  event_point, event_type tree_event_type) : event_point(event_point), tree_event_type(tree_event_type)
		{};
};

struct test_result {
	unsigned int true_positive = 0;
	unsigned int true_negative = 0;
	unsigned int false_positive = 0;
	unsigned int false_negative = 0;
};

test_result test_iterations(std::vector<tree_event> event_vector, Tree& tree_to_update)
{
	test_result result;
	for(auto it = event_vector.begin(); it != event_vector.end(); it++)
	{
		if((*it).tree_event_type == ADD)
			tree_to_update.add_point((*it).event_point);
		else if((*it).tree_event_type == DEL)
			tree_to_update.delete_point((*it).event_point);
		else
		{
			bool eval_result = tree_to_update.decision((*it).event_point.get_features());
			if(eval_result)
				if((*it).event_point.get_value())
					result.true_positive++;
				else
					result.false_positive++;
			else
				if((*it).event_point.get_value())
					result.false_negative++;
				else
					result.true_negative++;
		}
	}
	return result;
}

Tree from_file(std::string file_name,
                size_t dimension,
                char delimiter,
                unsigned int label_position,
                float label_true_value,
                std::vector<size_t> add_indices, std::vector<size_t> del_indices, std::vector<size_t> eval_indices,
                std::vector<tree_event> &event_vector)
{
    std::multiset<Point*> tree_points;
    auto it_add = add_indices.begin();
    auto it_del = del_indices.begin();
    auto it_eval = eval_indices.begin();
    std::ifstream data_file(file_name);
    std::string current_line;
    if (data_file.is_open()) {
        for(size_t i = 0;data_file; i++)
        {
            data_file >> current_line;
            size_t val_it_add = it_add == add_indices.end() ? UINTMAX_MAX : *it_add;
            size_t val_it_del = it_del == del_indices.end() ? UINTMAX_MAX : *it_del;
            size_t val_it_eval = it_eval == del_indices.end() ? UINTMAX_MAX : *it_eval;
            if(i == val_it_del || (i != val_it_add && i != val_it_eval))
			{
                Point* new_point = new Point(current_line, dimension, delimiter, label_position, label_true_value);
                tree_points.insert(new_point);
			}
			if(i == val_it_del)
			{
				tree_event new_event(Point(current_line, dimension, delimiter, label_position, label_true_value), DEL);
				event_vector.push_back(new_event);
				it_del++;
			}
			if(i == val_it_eval)
			{
				tree_event new_event(Point(current_line, dimension, delimiter, label_position, label_true_value), EVAL);
				event_vector.push_back(new_event);
				it_eval++;
			}
            if(i == val_it_add)
            {
				tree_event new_event(Point(current_line, dimension, delimiter, label_position, label_true_value), ADD);
				event_vector.push_back(new_event);
				it_add++;
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

	std::vector<param_setting> settings {
		param_setting(true,
			true,
			"file_name",
			"",
			"",
			"Name of the file containing data",
			""),
		param_setting(true,
			true,
			"dimension",
			"",
			"",
			"Dimension of the features for each point (not counting the label)",
			""),
		param_setting(false,
			false,
			"label_position",
			"-p",
			"--label_position",
			"Position of the label in the file (0 is the first data)",
			"0"),
		param_setting(false,
			false,
			"label_true_value",
			"-v",
			"--true_value",
			"Value of the label that will be considered as true",
			"1.0"),
		param_setting(false,
			false,
			"delimiter",
			"-d",
			"--delimiter",
			"Character that separate data in the file",
			";")
	};
	std::map<std::string, std::string> parsed_params = parse_param(settings, argc, argv);
	std::string file_name = parsed_params["file_name"];
	size_t dimension = (size_t)std::stoul(parsed_params["dimension"]);
	size_t label_position = (size_t)std::stoul(parsed_params["label_position"]);
	float label_true_value = std::stof(parsed_params["label_true_value"]);
	char delimiter = parsed_params["label_true_value"][0];

    std::vector<tree_event> event_vector;

    const auto t1 = std::chrono::high_resolution_clock::now();
    Tree tree_from_file = from_file(file_name,
                dimension,
                delimiter,
                label_position,
                label_true_value,
                std::vector<size_t> {1001, 1002, 1003, 1004, 1005}, std::vector<size_t> {/*1004, 1005, 1006, 1007, 1008*/}, std::vector<size_t> {1005, 1010, 1011, 1012, 1253}, event_vector);

     std::cout << tree_from_file.to_string();

     test_result result = test_iterations(event_vector, tree_from_file);
     std::cout << "TP : " << result.true_positive << "; TN : " << result.true_negative << std::endl;
     std::cout << "FP : " << result.false_positive << "; FN : " << result.false_negative << std::endl;

     std::cout << tree_from_file.to_string();

    const auto t2 = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double, std::milli> ms = t2 - t1;
    std::cout << "Execution time (ms) : " << ms.count() <<std::endl;
    return 0;
}
