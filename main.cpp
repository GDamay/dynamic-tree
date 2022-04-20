#include "param_parser.h"
#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <chrono>
#include <queue>
#include <random>
#include <algorithm>
#include "Models/PointSet/Point.h"
#include "Models/PointSet/PointSet.h"
#include "Models/Tree/Vertex.h"
#include "Models/Tree/Tree.h"

//constexpr auto DIMENSION = 3;
//constexpr float EPSILON = (float)0.2;
//constexpr auto MAX_HEIGHT = 5;
using namespace std;

enum class algo_type{SLIDING, RANDOM};

enum class event_type{ADD, DEL, EVAL};
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
		if((*it).tree_event_type == event_type::ADD)
			tree_to_update.add_point((*it).event_point);
		else if((*it).tree_event_type == event_type::DEL)
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

Tree random_from_file(std::string file_name,
				size_t dimension,
				char delimiter,
				size_t label_position,
				float label_true_value,
				unsigned int initial_size, double eval_proba,
				unsigned int number_of_updates, double insert_probability,
				unsigned int seed,
                std::vector<tree_event> &event_vector,
				bool skip_first_line,
				float epsilon,
				unsigned int max_height)
{
	// --- Reading file ---
	std::vector<Point> points_in_file;
    std::fstream data_file(file_name);
    std::string current_line;
    if (data_file.is_open()) {
		if(skip_first_line)
            getline(data_file, current_line);
        for(size_t i = 0; getline(data_file, current_line); i++)
			points_in_file.push_back(Point(current_line, dimension, delimiter, (unsigned int)label_position, label_true_value));

    }
    else
        throw "Error when oppening the data file";
    data_file.close();

	// --- Building events ---
	srand(seed);
    std::multiset<Point*> tree_points;
	std::multiset<Point> already_added_points;
    std::shuffle(points_in_file.begin(), points_in_file.end(), std::default_random_engine(seed));
    auto point_to_treat = points_in_file.begin();
	for(unsigned int i = 0; i < initial_size && point_to_treat != points_in_file.end(); i++, point_to_treat++)
	{
		Point* new_tree_point = new Point(*point_to_treat);
		tree_points.insert(new_tree_point);

		already_added_points.insert(Point(*point_to_treat));
	}
	if(point_to_treat != points_in_file.end())
		point_to_treat++;
	for(unsigned int i = 0; i < number_of_updates && point_to_treat != points_in_file.end(); i++, point_to_treat++)
	{
		if((double)rand()/(RAND_MAX) < eval_proba)
		{
			tree_event new_event(*point_to_treat, event_type::EVAL);
			event_vector.push_back(new_event);
		}
		if(already_added_points.size() == 0 || (double)rand()/(RAND_MAX) < insert_probability)
		{
			tree_event new_event(*point_to_treat, event_type::ADD);
			event_vector.push_back(new_event);

			already_added_points.insert(Point(*point_to_treat));
		}
		else
		{
			unsigned int rand_val = (unsigned int)(double)rand() / (RAND_MAX)*(already_added_points.size());
			auto to_del = already_added_points.begin();
			std::advance(to_del, rand_val);
			tree_event new_event(*to_del, event_type::DEL);
			event_vector.push_back(new_event);

			already_added_points.erase(to_del);
		}
	}
    return Tree(tree_points, dimension, max_height, epsilon);
}

Tree window_from_file(std::string file_name,
                size_t dimension,
                char delimiter,
                size_t label_position,
                float label_true_value,
                unsigned int window_size, double eval_proba,
				unsigned int seed,
                std::vector<tree_event> &event_vector,
				bool skip_first_line,
				float epsilon,
				unsigned int max_height)
{
    std::multiset<Point*> tree_points;
	std::queue<Point> points_to_delete;
	srand(seed);
    std::fstream data_file(file_name);
    std::string current_line;
    if (data_file.is_open()) {
		if(skip_first_line)
            getline(data_file, current_line);
        for(size_t i = 0; getline(data_file, current_line); i++)
        {
			points_to_delete.push(Point(current_line, dimension, delimiter, (unsigned int)label_position, label_true_value));
            if(i < window_size)
			{
                Point* new_point = new Point(current_line, dimension, delimiter, (unsigned int)label_position, label_true_value);
                tree_points.insert(new_point);
			}
			else
			{
				if(((double) rand() / (RAND_MAX)) < eval_proba)
				{
					tree_event new_event(Point(current_line, dimension, delimiter, (unsigned int)label_position, label_true_value), event_type::EVAL);
					event_vector.push_back(new_event);
				}
				tree_event del_event(points_to_delete.front(), event_type::DEL);
				event_vector.push_back(del_event);
				points_to_delete.pop();
				tree_event add_event(Point(current_line, dimension, delimiter, (unsigned int)label_position, label_true_value), event_type::ADD);
				event_vector.push_back(add_event);
			}
        }
    }
    else
        throw "Error when oppening the data file";
    data_file.close();
    return Tree(tree_points, dimension, max_height, epsilon);
}

// Needed for not having to make generic constructor for Tree
Tree branched_from_file(std::string file_name,
	size_t dimension,
	char delimiter,
	size_t label_position,
	float label_true_value,
	unsigned int dataset_size, double eval_proba,
	unsigned int number_of_updates, double insert_probability,
	unsigned int seed,
	std::vector<tree_event>& event_vector,
	bool skip_first_line,
	float epsilon,
	unsigned int max_height,
	algo_type type_of_building)
{
	if (type_of_building == algo_type::SLIDING)
		return window_from_file(file_name,
			dimension,
			delimiter,
			label_position,
			label_true_value,
			dataset_size,
			eval_proba,
			seed,
			event_vector,
			skip_first_line,
			epsilon,
			max_height);
	else
		return random_from_file(file_name,
			dimension,
			delimiter,
			label_position,
			label_true_value,
			dataset_size,
			eval_proba,
			number_of_updates,
			insert_probability,
			seed,
			event_vector,
			skip_first_line,
			epsilon,
			max_height);
}

Tree from_file(std::string file_name,
                size_t dimension,
                char delimiter,
                size_t label_position,
                float label_true_value,
                std::vector<size_t> add_indices, std::vector<size_t> del_indices, std::vector<size_t> eval_indices,
                std::vector<tree_event> &event_vector,
				bool skip_first_line,
				float epsilon,
				unsigned int max_height)
{
    std::multiset<Point*> tree_points;
    auto it_add = add_indices.begin();
    auto it_del = del_indices.begin();
    auto it_eval = eval_indices.begin();
    std::fstream data_file(file_name);
    std::string current_line;
    if (data_file.is_open()) {
		if(skip_first_line)
            getline(data_file, current_line);
        for(size_t i = 0; getline(data_file, current_line); i++)
        {
            size_t val_it_add = it_add == add_indices.end() ? UINTMAX_MAX : *it_add;
            size_t val_it_del = it_del == del_indices.end() ? UINTMAX_MAX : *it_del;
            size_t val_it_eval = it_eval == eval_indices.end() ? UINTMAX_MAX : *it_eval;
            if(i == val_it_del || (i != val_it_add && i != val_it_eval))
			{
                Point* new_point = new Point(current_line, dimension, delimiter, (unsigned int)label_position, label_true_value);
                tree_points.insert(new_point);
			}
			if(i == val_it_del)
			{
				tree_event new_event(Point(current_line, dimension, delimiter, (unsigned int)label_position, label_true_value), event_type::DEL);
				event_vector.push_back(new_event);
				it_del++;
			}
			if(i == val_it_eval)
			{
				tree_event new_event(Point(current_line, dimension, delimiter, (unsigned int)label_position, label_true_value), event_type::EVAL);
				event_vector.push_back(new_event);
				it_eval++;
			}
            if(i == val_it_add)
            {
				tree_event new_event(Point(current_line, dimension, delimiter, (unsigned int)label_position, label_true_value), event_type::ADD);
				event_vector.push_back(new_event);
				it_add++;
            }
        }
    }
    else
        throw "Error when oppening the data file";
    data_file.close();
    return Tree(tree_points, dimension, max_height, epsilon);
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
			";"),
		param_setting(false,
			false,
			"skip_first_line",
			"-s",
			"--skip",
			"Indicates that file has a header line",
			"",
			true),
		param_setting(false,
			false,
			"epsilon",
			"-e",
			"--epsilon",
			"Epsilon of algorithm, determining when to rebuild node",
			"0.2"),
		param_setting(false,
			false,
			"dataset_size",
			"-b",
			"--dataset_size",
			"Size of the window in SLIDING test or of the initial dataset in RANDOM mode",
			"3000"),
		param_setting(false,
			false,
			"eval_proba",
			"-a",
			"--proba",
			"Probability of a point after the window to be evaluation point",
			"0.01"),
		param_setting(false,
			false,
			"seed",
			"-r",
			"--seed",
			"Seed for choosing evaluation points (if -1 : random)",
			"-1"),
		param_setting(false,
			false,
			"max_height",
			"-h",
			"--max_height",
			"Max number of vertices between root and leaf (included)",
			"5"),
		param_setting(false,
			false,
			"test_type",
			"-t",
			"--type",
			"Type of test to make, between 'S' or 'SLIDING' for sliding window, and 'R' or 'RANDOM' for random unordered sampling",
			"S"),
		param_setting(false,
			false,
			"nb_updates",
			"-u",
			"--nb_updates",
			"Number of updates (add and del) to include in RANDOM test",
			"1000"),
		param_setting(false,
			false,
			"insert_proba",
			"-i",
			"--insert_proba",
			"Probability of each event in RANDOM test to be an insertion",
			"0.5"),
		param_setting(false,
			false,
			"is_output_csv",
			"-c",
			"--csv",
			"Indicates that the ouput should be formatted as CSV",
			"",
			true)
	};
	std::map<std::string, std::string> parsed_params;
	if(parse_param(settings, argc, argv, parsed_params))
		return 0;
	std::string file_name = parsed_params["file_name"];
	size_t dimension = (size_t)std::stoul(parsed_params["dimension"]);
	size_t label_position = (size_t)std::stoul(parsed_params["label_position"]);
	float label_true_value = std::stof(parsed_params["label_true_value"]);
	char delimiter = parsed_params["label_true_value"][0];
	bool skip_first_line = parsed_params["skip_first_line"] == BOOLEAN_TRUE_VALUE;
	float epsilon = std::stof(parsed_params["epsilon"]);
	unsigned int dataset_size = (unsigned int)std::stoul(parsed_params["dataset_size"]);
	float eval_proba = std::stof(parsed_params["eval_proba"]);
	unsigned int seed = parsed_params["seed"] == "-1" ? time(0) : (unsigned int)std::stoul(parsed_params["seed"]);
	unsigned int max_height = (unsigned int)std::stoul(parsed_params["max_height"]);
	algo_type current_algo_type;
	if(parsed_params["test_type"] == "S" || parsed_params["test_type"] == "SLIDING")
			current_algo_type = algo_type::SLIDING;
	else if(parsed_params["test_type"] == "R" || parsed_params["test_type"] == "RANDOM")
			current_algo_type = algo_type::RANDOM;
	else
			throw "Unknown algo type : " + parsed_params["test_type"];
	unsigned int nb_updates = (unsigned int)std::stoul(parsed_params["nb_updates"]);
	float insert_proba = std::stof(parsed_params["insert_proba"]);
	bool is_output_csv = parsed_params["is_output_csv"] == BOOLEAN_TRUE_VALUE;
    std::vector<tree_event> event_vector;

    const auto t1 = std::chrono::high_resolution_clock::now();
    /*Tree tree_from_file = from_file(file_name,
                dimension,
                delimiter,
                label_position,
                label_true_value,
                std::vector<size_t> {101, 102, 103, 104, 105}, std::vector<size_t> {1004, 1005, 1006, 1007, 1008}, std::vector<size_t> {105, 110, 111, 112, 253}, event_vector, skip_first_line, epsilon, max_height);
*/

	Tree tree_from_file = branched_from_file(file_name,
                dimension,
                delimiter,
                label_position,
                label_true_value,
				dataset_size,
				eval_proba,
				nb_updates,
				insert_proba,
				seed,
				event_vector,
				skip_first_line,
				epsilon,
				max_height,
				current_algo_type);

    const auto t2 = std::chrono::high_resolution_clock::now();

	if(!is_output_csv)
		std::cout << tree_from_file.to_string();

    const auto t3 = std::chrono::high_resolution_clock::now();
     test_result result = test_iterations(event_vector, tree_from_file);
    const auto t4 = std::chrono::high_resolution_clock::now();

	if(is_output_csv)
		std::cout << epsilon << ";" << result.true_positive << ";" << result.true_negative << ";" << result.false_positive << ";" << result.false_negative;
	else
	{
		std::cout << "TP : " << result.true_positive << "; TN : " << result.true_negative << std::endl;
		std::cout << "FP : " << result.false_positive << "; FN : " << result.false_negative << std::endl;

		std::cout << tree_from_file.to_string();
	}

    const std::chrono::duration<double, std::milli> init_time = t2 - t1;
    const std::chrono::duration<double, std::milli> iter_time = t4 - t3;
    if(is_output_csv)
		std::cout << ";" << init_time.count() << ";" << iter_time.count() << Vertex::nb_build << std::endl;
	else
	{
		std::cout << "Initialization time (ms) : " << init_time.count() <<std::endl;
		std::cout << "Iteration time (ms) : " << iter_time.count() <<std::endl;
		std::cout << "Nb builds : " << Vertex::nb_build << std::endl;
	}
    return 0;
}
