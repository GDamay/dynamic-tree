/**
 * @file main.cpp
 * Run the dynamic_tree program to test its performances
 *
 * @todo When available, add link to the article in README and in Doxyfile
 */
#include "param_parser.h"
#include <sstream>
#include <iostream>
#include <sstream>
#include <fstream>
#include <set>
#include <map>
#include <vector>
#include <chrono>
#include <queue>
#include <random>
#include <algorithm>
#include <stdexcept>
#include "Models/PointSet/Point.h"
#include "Models/PointSet/PointSet.h"
#include "Models/Tree/Vertex.h"
#include "Models/Tree/Tree.h"

using namespace std;

/// Represents the type of test to run
enum class algo_type{
	/** 
	 * Kind of tests in which the data are considered sequetially
	 *
	 * The data in the tree will then correspond to a sliding window over the
	 * input file data. At each step, the next point in the file is added and
	 * the oldest point is removed.
	 */
	SLIDING,
	/**
	 * Kind of test in which the data are considered random
	 * 
	 * The data in the tree will correspond to a random selection of the points
	 * of the input file. At each step, a new random point not used so far is
	 * added or a random point of the file is removed.
	 */
	RANDOM
};

/// Represents the usage to make of a point at a step
enum class event_type{
	/// Add the point to the tree
	ADD,
	/// Delete the point from the tree
	DEL,
	/// Use the point to evaluate if the tree gives the right answer
	EVAL};

/**
 * A step of the test loop
 *
 * Use a point to change or evaluate the tree
 */
struct tree_event{
	/// Point to use
	Point  event_point;
	/// Action to do with the point
	event_type tree_event_type;
	/**
	 * Constructor of tree_event
	 *
	 * @param event_point Point to use
	 * @param tree_event_type Action to do with the point
	 */
	tree_event( Point  event_point, event_type tree_event_type) : event_point(event_point), tree_event_type(tree_event_type)
	{};
};

/// Data defining the performance of the tree during a test sequence
struct test_result {
	/// Number of event_type.EVAL points rightly classified as True
	unsigned int true_positive = 0;
	/// Number of event_type.EVAL points rightly classified as False
	unsigned int true_negative = 0;
	/// Number of event_type.EVAL points wrongly classified as True
	unsigned int false_positive = 0;
	/// Number of event_type.EVAL points wrongly classified as False
	unsigned int false_negative = 0;
	/**
	 * Sum of training error (number points of the graph in a leaf that would
	 * have classified them wrongly) at each evaluation step
	 */
	unsigned long total_training_error = 0;
};

/**
 * Read the header line of file, containing types of features and label position
 *
 * See the help of the program for more informations
 *
 * @param data_file In/out argument. Already-opened file reader from which the
 *  line will be red. The reader should be positioned at the beginning of the
 *  header line, and the fonction will leave it at the beginning of the
 *  following line
 * @param delimiter Char separating the items of the line
 * @param label_position Out argument, will be set to the position of the
 *  label in each line (first position is 0)
 * @param features_types In/out argument, the vector should be initialized but
 *  empty, and it will contain the ordered list of types of the features. If it
 *  is not empty, the list will be added after its last element
 * @return The number of features (dimension of the points)
 */
size_t read_header(std::fstream &data_file, char delimiter, size_t &label_position, std::vector<FeatureType> &features_types)
{
	size_t dimension;
	std::string current_line;
	std::string parsed;
	std::stringstream current_line_stream;
	bool label_position_found = false;
	if(getline(data_file, current_line))
	{
		current_line_stream = stringstream(current_line);
		for(dimension = 0; getline(current_line_stream,parsed, delimiter); dimension++)
		{
			if(parsed == "l")
			{
				if(label_position_found)
					throw std::runtime_error("Error : label identifier found twice in the header (should contain exactly 1 'l' identifier");
				label_position = dimension;
				label_position_found=true;
			}
			else if(parsed == "n")
				features_types.push_back(FeatureType::REAL);
			else if(parsed == "c")
				features_types.push_back(FeatureType::CLASSIFIED);
			else if(parsed == "b")
				features_types.push_back(FeatureType::BINARY);
			else
				throw std::runtime_error("Error : found unknown item identifier in header : \"" + parsed + "\"");
		}
	}
	else
		throw std::runtime_error("Error : file is empty or contains only line to skip");

	if(!label_position_found)
		throw std::runtime_error("Error : no label identifier found in the header (should contain exactly 1 'l' identifier");
	
	return dimension - 1;
}

/**
 * Parse current_line to a Point
 *
 * current_line should be formatted as a line of the input file
 *
 * @param current_line Line to parse
 * @param delimiter Char separating the different elements in the line
 * @param dimension Expected number of features (excluding the label) in the
 *  line
 * @param label_position The index of the position at which the label is
 *  expected to be found (first position is index 0)
 * @param features_types Ordered list of the features (excluding the label)
 * @param label_true_value String that the label should be equal to in the line
 *  to be counted as "True"
 * @param class_txt_to_index In/out argument, vector of dict containing, for
 *  each feature and when this feature is not of type "REAL", the list of values
 *  that have already been encountered for this feature and the index of the
 *  classes that are attributed . It will be updated if new values are
 *  encountered. It should at least be initialised with an empty map for each
 *  feature.
 * @param next_classification_id In/out argument, for each feature the value is
 *  the class that should be associated with a new value that would be
 *  encountered. If not used so far, it should be initialised with 0 for each
 *  feature.
 * @return The point of which features and labels are parsed from the line
 */
Point point_from_line(std::string current_line,
									char delimiter,
									size_t dimension,
									size_t label_position,
									std::vector<FeatureType> features_types,
									std::string label_true_value,
									std::vector<std::map<std::string, float>> &class_txt_to_index,
									std::vector<float> &next_classification_id)
{
    std::string parsed;
	float* features = new float[dimension];
	bool current_point_value;
	std::stringstream current_line_stream(current_line);
	float related_val;
	size_t j;
	for(j = 0; getline(current_line_stream,parsed, delimiter); j++)
	{
		if(j > dimension)
			throw std::runtime_error("Error : too many data");
		if(j==label_position)
			current_point_value = parsed==label_true_value;
		else
		{
			FeatureType current_feature_type = features_types[j - (label_position<j)];
			if(current_feature_type == FeatureType::REAL)
				related_val = std::stof(parsed);
			else
			{
				auto related_val_it = class_txt_to_index[j].find(parsed);
				if(related_val_it == class_txt_to_index[j].end())
				{
					related_val = next_classification_id[j];
					next_classification_id[j]++;
					class_txt_to_index[j][parsed] = related_val;
					if(current_feature_type == FeatureType::BINARY && related_val > 1)
						throw std::runtime_error("Error : at least 3 different values found for a feature supposed to be binary");
				}
				else
					related_val = related_val_it->second;
			}
			features[j - (label_position < j)] = related_val;
		}
	}
	if (j < dimension + 1)
		throw std::runtime_error("Error : too few dimensions");
	Point to_return(dimension, features, current_point_value);
	delete [] features;
	return to_return;
}

/**
 * Run the test steps of event_vector
 *
 * @param event_vector Ordered list of events to perform
 * @param tree_to_update Tree on which performing the events
 * @return Data of the EVAL events
 */
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
			result.total_training_error += tree_to_update.get_training_error();
		}
	}
	return result;
}

/**
 * Create a random test sequence from the data of the file
 *
 * @param file_name Path to the input file (absolute or relative)
 * @param label_true_value Value for which, when label is equal to it, the label
 *  will be considered equal to True
 * @param delimiter Char separating the different elements of a same line
 * @param initial_size Number of points to include in the original tree, before
 *  performing any event
 * @param eval_proba For each event of type ADD or DEL, probability of
 *  performing an EVAL event before (if the event is ADD, the EVAL will be made
 *  using the point to add)
 * @param number_of_updates Number of events of type update (ADD or DEL) to
 *  perform (could be lowered if there are no points left)
 * @param insert_probability Probability of an update event (ADD or DEL) to be
 *  ADD. If this is > 0.5, the expected size of the tree will grow, and if it is
 *  < 0.5, the expected size will shrink
 * @param seed The seed to use for the random operations
 * @param event_vector Out argument, vector of the events. It is expected to be
 *  empty, if it is not the events will be added at the end.
 * @param skip_first_line If true, the first line of the file will be considered
 *  irrelevant and skipped (for exemple, labels of features)
 * @param epsilon The epsilon value to use (see paper)
 * @param max_height The maximum size the tree should be able to reach (at init
 *  and during updated) (see paper, parameter h)
 * @param min_split_points Minimal number of points a node of the tree should
 *  contain to not be a leaf (at init and during updates) (see paper, parameter 
 *  k)
 * @param min_split_gini Minimal gini value a node of the tree should have to
 *  not be a leaf (at init and during updates) (see paper, parameter alpha)
 * @param epsilon_transmission Epsilon value to use when searching which parent
 *  node to recompute (line 11 of the algorithm 1). It is usually equal to
 *  epsilon but could be changed for tests.
 * @return The inital tree on which to perform the events
 */
Tree random_from_file(std::string file_name,
				std::string label_true_value,
				char delimiter,
				unsigned int initial_size, double eval_proba,
				unsigned int number_of_updates, double insert_probability,
				unsigned int seed,
                std::vector<tree_event> &event_vector,
				bool skip_first_line,
				float epsilon,
				unsigned int max_height,
				unsigned int min_split_points,
				float min_split_gini,
				float epsilon_transmission)
{
	// --- Reading file ---
	size_t dimension;
	size_t label_position;
	std::vector<Point> points_in_file;
    std::fstream data_file(file_name);
    std::string current_line;
	std::vector<FeatureType> features_types;

    if (data_file.is_open()) {
		if(skip_first_line)
            getline(data_file, current_line);

        dimension = read_header(data_file, delimiter, label_position, features_types);

		std::vector<float> next_classification_id(dimension+1, 0.0);
		std::vector<std::map<std::string, float>> class_txt_to_index(dimension+1, std::map<std::string, float>());

        for(size_t i = 0; getline(data_file, current_line); i++)
			points_in_file.push_back(point_from_line(current_line, delimiter, dimension, label_position, features_types, label_true_value, class_txt_to_index, next_classification_id));

    }
    else
        throw std::runtime_error("Error when oppening the data file");
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
    return Tree(tree_points, dimension, max_height, epsilon, min_split_points, min_split_gini, epsilon_transmission, features_types);
}

/**
 * Create a sliding window test sequence from the data of the file
 *
 * @param file_name Path to the input file (absolute or relative)
 * @param label_true_value Value for which, when label is equal to it, the label
 *  will be considered equal to True
 * @param delimiter Char separating the different elements of a same line
 * @param window_size Number of points to include in the original tree, and that
 *  will be in the tree after each sequence of DEL and ADD
 * @param eval_proba For each sequence of DEL and ADD, probability of
 *  performing an EVAL event before (the EVAL will be made using the point to
 *  add)
 * @param seed The seed to use for the random operations
 * @param event_vector Out argument, vector of the events. It is expected to be
 *  empty, if it is not the events will be added at the end.
 * @param skip_first_line If true, the first line of the file will be considered
 *  irrelevant and skipped (for exemple, labels of features)
 * @param epsilon The epsilon value to use (see paper)
 * @param max_height The maximum size the tree should be able to reach (at init
 *  and during updated) (see paper, parameter h)
 * @param min_split_points Minimal number of points a node of the tree should
 *  contain to not be a leaf (at init and during updates) (see paper, parameter 
 *  k)
 * @param min_split_gini Minimal gini value a node of the tree should have to
 *  not be a leaf (at init and during updates) (see paper, parameter alpha)
 * @param epsilon_transmission Epsilon value to use when searching which parent
 *  node to recompute (line 11 of the algorithm 1). It is usually equal to
 *  epsilon but could be changed for tests.
 * @return The inital tree on which to perform the events
 */
Tree window_from_file(std::string file_name,
				std::string label_true_value,
                char delimiter,
                unsigned int window_size, double eval_proba,
				unsigned int seed,
                std::vector<tree_event> &event_vector,
				bool skip_first_line,
				float epsilon,
				unsigned int max_height,
				unsigned int min_split_points,
				float min_split_gini,
				float epsilon_transmission)
{
	size_t dimension;
	size_t label_position;
    std::multiset<Point*> tree_points;
	std::queue<Point> points_to_delete;
	srand(seed);
    std::fstream data_file(file_name);
    std::string current_line;
	std::vector<FeatureType> features_types;

    if (data_file.is_open()) {
		if(skip_first_line)
            getline(data_file, current_line);

        dimension = read_header(data_file, delimiter, label_position, features_types);

		std::vector<float> next_classification_id(dimension+1, 0.0);
		std::vector<std::map<std::string, float>> class_txt_to_index(dimension+1, std::map<std::string, float>());
 
        for(size_t i = 0; getline(data_file, current_line); i++)
        {
			Point current_point = point_from_line(current_line, delimiter, dimension, label_position, features_types, label_true_value, class_txt_to_index, next_classification_id);
			points_to_delete.push(Point(current_point));
            if(i < window_size)
			{
                Point* new_point = new Point(current_point);
                tree_points.insert(new_point);
			}
			else
			{
				if(((double) rand() / (RAND_MAX)) < eval_proba)
				{
					tree_event new_event(Point(current_point), event_type::EVAL);
					event_vector.push_back(new_event);
				}
				tree_event del_event(points_to_delete.front(), event_type::DEL);
				event_vector.push_back(del_event);
				points_to_delete.pop();
				tree_event add_event(Point(current_point), event_type::ADD);
				event_vector.push_back(add_event);
			}
        }
    }
    else
        throw std::runtime_error("Error when oppening the data file");
    data_file.close();
    return Tree(tree_points, dimension, max_height, epsilon, min_split_points, min_split_gini, epsilon_transmission, features_types);
}

/**
 * Create a sliding window test sequence from the data of the file
 *
 * This is needed for not having to make generic constructor for Tree
 *
 * @param file_name Path to the input file (absolute or relative)
 * @param label_true_value Value for which, when label is equal to it, the label
 *  will be considered equal to True
 * @param delimiter Char separating the different elements of a same line
 * @param dataset_size Number of points to include in the original tree, and in
 *  the case of the sliding window experiment, it will be the number of points
 *  in the tree after each sequence of DEL and ADD
 * @param eval_proba For each sequence of DEL and ADD (case of sliding window), 
 *  or each update event (case of random test), probability of performing an 
 *  EVAL event before (the EVAL will be made using the point to add)
 * @param number_of_updates Number of events of type update (ADD or DEL) to
 *  perform (could be lowered if there are no points left). Relevant only for
 *  tests of the random kind.
 * @param insert_probability Probability of an update event (ADD or DEL) to be
 *  ADD. If this is > 0.5, the expected size of the tree will grow, and if it is
 *  < 0.5, the expected size will shrink. Relevent only for tests of the random
 *  kind.
 * @param seed The seed to use for the random operations
 * @param event_vector Out argument, vector of the events. It is expected to be
 *  empty, if it is not the events will be added at the end.
 * @param skip_first_line If true, the first line of the file will be considered
 *  irrelevant and skipped (for exemple, labels of features)
 * @param epsilon The epsilon value to use (see paper)
 * @param max_height The maximum size the tree should be able to reach (at init
 *  and during updated) (see paper, parameter h)
 * @param type_of_building Define type of test to generate (SLIDING or RANDOM)
 * @param min_split_points Minimal number of points a node of the tree should
 *  contain to not be a leaf (at init and during updates) (see paper, parameter 
 *  k)
 * @param min_split_gini Minimal gini value a node of the tree should have to
 *  not be a leaf (at init and during updates) (see paper, parameter alpha)
 * @param epsilon_transmission Epsilon value to use when searching which parent
 *  node to recompute (line 11 of the algorithm 1). It is usually equal to
 *  epsilon but could be changed for tests.
 * @return The inital tree on which to perform the events
 */
Tree branched_from_file(std::string file_name,
	char delimiter,
	std::string label_true_value,
	unsigned int dataset_size, double eval_proba,
	unsigned int number_of_updates, double insert_probability,
	unsigned int seed,
	std::vector<tree_event>& event_vector,
	bool skip_first_line,
	float epsilon,
	unsigned int max_height,
	algo_type type_of_building,
	unsigned int min_split_points,
	float min_split_gini,
	float epsilon_transmission)
{
	if (type_of_building == algo_type::SLIDING)
		return window_from_file(file_name,
			label_true_value,
			delimiter,
			dataset_size,
			eval_proba,
			seed,
			event_vector,
			skip_first_line,
			epsilon,
			max_height,
			min_split_points,
			min_split_gini,
			epsilon_transmission);
	else
		return random_from_file(file_name,
			label_true_value,
			delimiter,
			dataset_size,
			eval_proba,
			number_of_updates,
			insert_probability,
			seed,
			event_vector,
			skip_first_line,
			epsilon,
			max_height,
			min_split_points,
			min_split_gini,
			epsilon_transmission);
}

/// Main function, see help (run program with argument '--help')
int main(int argc, char *argv[])
{
	std::vector<param_setting> settings {
		param_setting(true,
			true,
			"file_name",
			"",
			"",
			"Name of the file containing data",
			""),
		param_setting(false,
			false,
			"label_true_value",
			"-v",
			"--true_value",
			"Value of the label that will be considered as true",
			"1"),
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
			"Indicates that file has a header line before the one describing fields types",
			"",
			true),
		param_setting(false,
			false,
			"epsilon",
			"-e",
			"--epsilon",
			"Epsilon of algorithm, determining when to rebuild node. If -1, will be calculated as min(min_split_gini/6, 1/(min_split_points+2))",
			"-1"),
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
			true),
		param_setting(false,
			false,
			"min_split_points",
			"-m",
			"--min_split_points",
			"Minimal number of points in a vertex to make it have children",
			"0"),
		param_setting(false,
			false,
			"min_split_gini",
			"-g",
			"--min_split_gini",
			"Minimal gini value of the points set of a vertex to make it have children",
			"0"),
		param_setting(false,
			false,
			"epsilon_transmission",
			"-w",
			"--epsilon_transmission",
			"Epsilon to apply when choosing which layer to recompute. If -1 : epsilon",
			"-1"),
		param_setting(false,
			false,
			"epsilon_max",
			"-f",
			"--epsilon_max",
			"For making several tests, set this to the max epsilon to test. If -1 : epsilon",
			"-1"),
		param_setting(false,
			false,
			"epsilon_step",
			"-j",
			"--epsilon_step",
			"For making several tests, set this to the step between epsilons to test",
			"0.1")
	};
	std::string help_message = std::string("This program runs the Dynamic Decision Tree algorithm (see [paper to be published]).\n")
	+ std::string("The input file MUST be a csv featuring an header with as many columns as the rest of the file. Each column MUST contain a single character, 'l' if the column contains the decision label, 'n' if the column is numerical, 'b' if the column is binary and 'c' if it is categorial. The file MAY also feature a single line containing data irrelevant for the algorithm, in which case this line MUST be at the beginning of the file, before the header, and the '-s' argument MUST be provided.\n")
	+ std::string("If the '-c' argument is provided, the output will be formatted as csv without header. If several runs are made using various values for epsilon, each run will correspond to a single output line. The csv columns will be \n")
	+ std::string(" - The seed used for the run\n")
	+ std::string(" - The value of epsilon for this run\n")
	+ std::string(" - The number of True Positive results of the tests\n")
	+ std::string(" - The number of True Negative results of the tests\n")
	+ std::string(" - The number of False Positive results of the tests\n")
	+ std::string(" - The number of False Negative results of the tests\n")
	+ std::string(" - The time (in ms) spent for initializing the tree\n")
	+ std::string(" - The total time (in ms) spent for making all the iterations of this run\n")
	+ std::string(" - The number of vertices (including leaves) that have been rebuild during the iterations\n")
	+ std::string(" - The mean training error, evaluated along all iterations\n")
	+ std::string("An header of the output csv could therefore be :\n")
	+ std::string("seed;current_epsilon;true_positive;true_negative;false_positive;false_negative;init_time;iter_time;nb_build;mean_training_error");

	std::map<std::string, std::string> parsed_params;
	if(parse_param(help_message, settings, argc, argv, parsed_params))
		return 0;
	std::string file_name = parsed_params["file_name"];
	std::string label_true_value = parsed_params["label_true_value"];
	char delimiter = parsed_params["delimiter"][0];
	bool skip_first_line = parsed_params["skip_first_line"] == BOOLEAN_TRUE_VALUE;
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
			throw std::runtime_error("Unknown algo type : " + parsed_params["test_type"]);
	unsigned int nb_updates = (unsigned int)std::stoul(parsed_params["nb_updates"]);
	float insert_proba = std::stof(parsed_params["insert_proba"]);
	bool is_output_csv = parsed_params["is_output_csv"] == BOOLEAN_TRUE_VALUE;
	unsigned int min_split_points = (unsigned int)std::stoul(parsed_params["min_split_points"]);
	float min_split_gini = std::stof(parsed_params["min_split_gini"]);
	float epsilon_step = std::stof(parsed_params["epsilon_step"]);
	float epsilon = parsed_params["epsilon"] == "-1" ? std::min(min_split_gini/6,  float(1)/(min_split_points + 2)) : std::stof(parsed_params["epsilon"]);
	float epsilon_transmission = parsed_params["epsilon_transmission"] == "-1" ? epsilon : std::stof(parsed_params["epsilon_transmission"]);
	float epsilon_max = parsed_params["epsilon_max"] == "-1" ? epsilon : std::stof(parsed_params["epsilon_max"]);
    std::vector<tree_event> event_vector;

    const auto t1 = std::chrono::high_resolution_clock::now();

	Tree reference_tree = branched_from_file(file_name,
                delimiter,
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
				current_algo_type,
				min_split_points,
				min_split_gini,
				epsilon_transmission);

    const auto t2 = std::chrono::high_resolution_clock::now();

	if(!is_output_csv)
		std::cout << reference_tree.to_string();

	for(float current_epsilon = epsilon; current_epsilon <= epsilon_max; current_epsilon += epsilon_step)
	{
		epsilon_transmission = parsed_params["epsilon_transmission"] == "-1" ? current_epsilon : epsilon_transmission;
		Tree current_tree(reference_tree, current_epsilon, epsilon_transmission);
		Vertex::reset_nb_build();
		const auto t3 = std::chrono::high_resolution_clock::now();
		 test_result result = test_iterations(event_vector, current_tree);
		const auto t4 = std::chrono::high_resolution_clock::now();

		if(is_output_csv)
			std::cout << seed << ";" << current_epsilon << ";" << result.true_positive << ";" << result.true_negative << ";" << result.false_positive << ";" << result.false_negative;
		else
		{
			std::cout << "TP : " << result.true_positive << "; TN : " << result.true_negative << std::endl;
			std::cout << "FP : " << result.false_positive << "; FN : " << result.false_negative << std::endl;

			std::cout << current_tree.to_string();
		}

		const std::chrono::duration<double, std::milli> init_time = t2 - t1;
		const std::chrono::duration<double, std::milli> iter_time = t4 - t3;
		if(is_output_csv)
			std::cout << ";" << init_time.count() << ";" << iter_time.count() << ";" << Vertex::get_nb_build() << ";" <<  (double)result.total_training_error / (result.true_positive + result.true_negative + result.false_positive + result.false_negative) << std::endl;
		else
		{
			std::cout << "Initialization time (ms) : " << init_time.count() <<std::endl;
			std::cout << "Iteration time (ms) : " << iter_time.count() <<std::endl;
			std::cout << "Nb builds : " << Vertex::get_nb_build() << std::endl;
			std::cout << "Mean training error : " << (double)result.total_training_error / (result.true_positive + result.true_negative + result.false_positive + result.false_negative) << std::endl;
		}
	}
    return 0;
}
