/**
 * @file param_parser.h
 *  Defines a class to handle command-line parameters
 */
#ifndef PARAMPARSER_H_INCLUDED
#define PARAMPARSER_H_INCLUDED

#define SEPARATING_NAMES_MANDATORY 30
#define SEPARATING_MANDATORY_DESC 12

#define BOOLEAN_TRUE_VALUE "1"
#define BOOLEAN_FALSE_VALUE "0"

#include <vector>
#include <map>
#include <math.h>
#include <string>
#include <iostream>
#include <stdexcept>

/**
 * Structure defining the components of a command-line parameter
 */
struct param_setting {
	/**
	 * True when no default value can be given to the argument
	 *
	 * If a mandatory parameter is not provided, the program will not be able to
	 * run (except for displaying help).
	 */
	bool is_mandatory;
	/**
	 * True if the parameter is positional
	 *
	 * The parameter is positional if it is recognised by its position in the
	 * command line. The non-positional parameters are recognised by the
	 * parameter identifier given before. E.g. in command 
	 * './test_prog.exe bla -f blo.txt', parameter 'bla' is positional while
	 * parameter 'blo' is not positional (its identifier is '-f')
	 */
	bool is_positional;

	///Name displayed in help and key in returned dictionnary
	std::string return_name;

	/**
	 * Short (usually 1 letter) identifier of the parameter
	 *
	 * Only relevent for non-positional arguments. When defining the argument,
	 * this short identifier will be preceeded by a single dash (e.g. '-f')
	 */
	std::string short_name;

	/**
	 * Long identifier of the parameter
	 *
	 * Only relevent for non-positional arguments. When defining the argument,
	 * this long identifier will be preceeded by a double dash (e.g. '--file')
	 */
	std::string long_name;

	/// Description of the parameter, to display in help
	std::string description;

	/**
	 * Default value to set to the parameter when it is not given as argument
	 *
	 * Only relevent for not-mandatory parameters
	 */
	std::string default_value;

	/**
	 * True if the argument is a boolean
	 *
	 * If true, the argument is automatically not-mandatory and, when given as
	 * argument, only the identifier (with dashes) needs to be given. If the
	 * identifier is found, the parameter will be "1", else it will be "0".
	 */
	bool is_boolean; // If true, the parameter has just to be present to set value True (default is then automatically "false")
	/**
	 * Constructor of a param_setting object
	 *
	 * @param is_mandatory True when no default value can be given to the 
	 *  argument.
	 *  If a mandatory parameter is not provided, the program will not be able
	 *  to run (except for displaying help).
	 * @param is_positional True if the parameter is positional.
	 *  The parameter is positional if it is recognised by its position in the
	 *  command line. The non-positional parameters are recognised by the
	 *  parameter identifier given before. E.g. in command 
	 *  './test_prog.exe bla -f blo.txt', parameter 'bla' is positional while
	 *  parameter 'blo' is not positional (its identifier is '-f')
	 * @param return_name Name displayed in help and key in returned dictionnary
	 * @param short_name Short (usually 1 letter) identifier of the parameter.
	 *  Only relevent for non-positional arguments. When defining the argument,
	 *  this short identifier will be preceeded by a single dash (e.g. '-f')
	 * @param long_name Long identifier of the parameter.
	 *  Only relevent for non-positional arguments. When defining the argument,
	 *  this long identifier will be preceeded by a double dash (e.g. '--file')
	 * @param description Description of the parameter, to display in help
	 * @param default_value Default value to set to the parameter when it is not
	 *  given as argument. Only relevent for not-mandatory parameters
	 * @param is_boolean True if the argument is a boolean. 
	 *  If true, the argument is automatically not-mandatory and, when given as
	 *  argument, only the identifier (with dashes) needs to be given. If the
	 *  identifier is found, the parameter will be "1", else it will be "0".
	 */
	param_setting(bool is_mandatory,
					bool is_positional,
					std::string return_name,
					std::string short_name,
					std::string long_name,
					std::string description,
					std::string default_value = "",
					bool is_boolean = false):
		is_mandatory(is_mandatory),
		is_positional(is_positional),
		return_name(return_name),
		short_name(short_name),
		long_name(long_name),
		description(description),
		default_value(default_value),
		is_boolean(is_boolean) {};
};

/**
 * Internal function, meant for finding the word searched_obj in the list of
 * words argv
 *
 * @param argv The list of words to search
 * @param begin Index of the first element to look in the list
 * @param size The size of the list
 * @param searched_obj The word searched
 * @return The position of the first occurence if the word has been found, else
 *  size
 */
size_t find(char *argv[], size_t begin, size_t size, std::string searched_obj)
{
	for(size_t i = begin; i < size; i++)
		if(argv[i] == searched_obj)
			return i;
	return size;
}

// Return true if param "--help" was given, and therefore program has to return
/**
 * Function parsing the parameters and filling parsed_params with the values
 *
 * @param help_message General help message that will be displayed if param
 *  '--help' is found
 * @param settings List of param_setting defining the expected parameters. The
 *  mandatory positional parameters should come first, then the non-mandatory
 *  positional parameters, and then the non-positional parameters.
 * @param argc Size of the argv list
 * @param argv List of arguments, splitted by words
 * @param parsed_param In/Out dict of which values corresponding to keys that
 *  are the return_name of the parameters will be set to the value of the
 *  related parameter
 * @return True if the '--help' argument is found, and hence the help is
 *  displayed, the parsed_param dict is not filled and the program should stop
 *  immediately after exiting the function.
 */
bool parse_param(std::string help_message, std::vector<param_setting> settings, int argc, char *argv[], std::map<std::string, std::string> &parsed_params)
{
	size_t pos = find(argv, 0, argc, "--help");
	// Help argument found
	if(pos < argc)
	{
		// ----------- Display help -----------
		// Only print help and quit, don't execute the program itself
		// ---- Print usage ----
		std::cout << "Usage: " << argv[0];
		for(auto it = settings.begin(); it != settings.end() && (*it).is_positional; it++)
			std::cout << " " << (*it).return_name;
		std::cout << " " << "[OPTIONS]" << std::endl << std::endl;

		// ---- Print general help message ----
		std::cout << help_message << std::endl << std::endl;

		// ---- Print positional arguments ----
		std::cout << "Positional arguments:" <<std::endl;
		auto it = settings.begin();
		for(; it != settings.end() && (*it).is_positional; it++)
			std::cout << "  " << (*it).return_name
					<< std::string(std::max(1, (int)(SEPARATING_NAMES_MANDATORY - (*it).return_name.length())), ' ')
					<< ((*it).is_mandatory
							? "mandatory" + std::string(std::max(1, SEPARATING_MANDATORY_DESC - (int)((std::string)"mandatory").length()), ' ')
							: std::string(std::max(1, SEPARATING_MANDATORY_DESC), ' '))
					<< (*it).description << ((*it).default_value != "" ? " (Default: " + (*it).default_value + ")" : "") << std::endl;
		// ---- Print non-positional arguments ----
		std::cout<<std::endl << "Options:" <<std::endl;
		for(; it != settings.end(); it++)
			std::cout << "  " << (*it).short_name << ", " << (*it).long_name
					<< std::string(std::max(1, SEPARATING_NAMES_MANDATORY - (int)(*it).short_name.length() - (int)(*it).long_name.length() - (int)((std::string)", ").length()), ' ')
					<< ((*it).is_mandatory
							? "mandatory" + std::string(std::max(1, SEPARATING_MANDATORY_DESC - (int)((std::string)"mandatory").length()), ' ')
							: std::string(std::max(1, SEPARATING_MANDATORY_DESC), ' '))
					<< (*it).description << ((*it).default_value != "" && !(*it).is_boolean ? " (Default: " + (*it).default_value + ")" : "") << std::endl;
		return true;

	}
	// ----------- Parse arguments -----------
	/// Set to false when the first non-positional argument has been studied
	bool are_still_positional = true;
	/**
     * Iterator of the words of argv
	 *
	 * Only used for positional arguments (non-positional arguments are always
	 * search for in the whole arguments list)
	 */
	size_t i = 1;
	// While we are still expecting arguments
	for(auto it = settings.begin(); it != settings.end(); it++)
	{
		// ---- Parsing positional arguments ----
		// If a non-positional argument has already been studied but there are
		// positional arguments after it in the list
		if(!are_still_positional && (*it).is_positional)
		{
			std::cerr << "Error : all positional arguments should be at the beginning of the setting" << std::endl;
			throw std::runtime_error("Error : all positional arguments should be at the beginning of the setting");
		}
		are_still_positional = (*it).is_positional;
		if(are_still_positional)
		{
			// There are still arguments and the flag for arguments identifier
			// is not found (therefore it is supposed to be a positional
			// argument
			if(i < argc && argv[i][0] != '-')
			{
				parsed_params[(*it).return_name] = argv[i];
				i++;
			}
			else if(!(*it).is_mandatory)
				parsed_params[(*it).return_name] = (*it).default_value;
			// Positional argument expected but no or non-positional argument
			// given
			else
			{
				std::cerr << "Error: Missing required positional parameter" << std::endl;
				throw std::runtime_error("Error: Missing required positional parameter");
			}
		}
		// ---- Parsing non-positional arguments ----
		else
		{
			pos = find(argv, i, argc, (*it).short_name);
			if (pos>=argc)
			{
				pos = find(argv, i, argc, (*it).long_name);
			}
			// If found
			if(pos<argc)
			{
				if((*it).is_boolean)
					parsed_params[(*it).return_name] = BOOLEAN_TRUE_VALUE;
				else
				{
					pos++;
					parsed_params[(*it).return_name] = argv[pos];
				}
			}
			// If not found but is boolean
			else if((*it).is_boolean)
				parsed_params[(*it).return_name] = BOOLEAN_FALSE_VALUE;
			// If not found but is not mandatory
			else if(!(*it).is_mandatory)
				parsed_params[(*it).return_name] = (*it).default_value;
			else
			{
				std::cerr << "Error: Missing required non-positional parameter" << std::endl;
				throw std::runtime_error("Error: Missing required non-positional parameter");
			}
		}
	}
	return false;
}
#endif // PARAMPARSER_H_INCLUDED
