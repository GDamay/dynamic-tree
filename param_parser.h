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

struct param_setting {
	bool is_mandatory;
	bool is_positional;
	std::string return_name;
	std::string short_name;
	std::string long_name;
	std::string description;
	std::string default_value;
	bool is_boolean; // If true, the parameter has just to be present to set value True (default is then automatically "false")
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

size_t find(char *argv[], size_t begin, size_t size, std::string searched_obj)
{
	for(size_t i = begin; i < size; i++)
		if(argv[i] == searched_obj)
			return i;
	return size;
}

// Return true if param "--help" was given, and therefore program has to return
bool parse_param(std::vector<param_setting> settings, int argc, char *argv[], std::map<std::string, std::string> &parsed_params)
{
	size_t pos = find(argv, 0, argc, "--help");
	if(pos < argc)
	{
		std::cout << "Usage: " << argv[0];
		for(auto it = settings.begin(); it != settings.end() && (*it).is_positional; it++)
			std::cout << " " << (*it).return_name;
		std::cout << " " << "[OPTIONS]" << std::endl << std::endl;

		std::cout << "Positional arguments:" <<std::endl;
		auto it = settings.begin();
		for(; it != settings.end() && (*it).is_positional; it++)
			std::cout << "  " << (*it).return_name
					<< std::string(std::max(1, (int)(SEPARATING_NAMES_MANDATORY - (*it).return_name.length())), ' ')
					<< ((*it).is_mandatory
							? "mandatory" + std::string(std::max(1, SEPARATING_MANDATORY_DESC - (int)((std::string)"mandatory").length()), ' ')
							: std::string(std::max(1, SEPARATING_MANDATORY_DESC), ' '))
					<< (*it).description << ((*it).default_value != "" ? " (Default: " + (*it).default_value + ")" : "") << std::endl;
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
	bool are_still_positional = true;
	size_t i = 1;
	for(auto it = settings.begin(); it != settings.end(); it++)
	{
		if(!are_still_positional && (*it).is_positional)
		{
			std::cerr << "Error : all positional arguments should be at the beginning of the setting" << std::endl;
			throw "Error : all positional arguments should be at the beginning of the setting";
		}
		are_still_positional = (*it).is_positional;
		if(are_still_positional)
		{
			if(i < argc && argv[i][0] != '-')
			{
				parsed_params[(*it).return_name] = argv[i];
				i++;
			}
			else if(!(*it).is_mandatory)
				parsed_params[(*it).return_name] = (*it).default_value;
			else
			{
				std::cerr << "Error: Missing required positional parameter" << std::endl;
				throw "Error: Missing required positional parameter";
			}
		}
		else
		{
			pos = find(argv, i, argc, (*it).short_name);
			if (pos>=argc)
			{
				pos = find(argv, i, argc, (*it).long_name);
			}
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
			else if((*it).is_boolean)
				parsed_params[(*it).return_name] = BOOLEAN_FALSE_VALUE;
			else if(!(*it).is_mandatory)
				parsed_params[(*it).return_name] = (*it).default_value;
			else
			{
				std::cerr << "Error: Missing required non-positional parameter" << std::endl;
				throw "Error: Missing required non-positional parameter";
			}
		}
	}
	return false;
}
#endif // PARAMPARSER_H_INCLUDED
