#ifndef PARAMPARSER_H_INCLUDED
#define PARAMPARSER_H_INCLUDED

#include <vector>
#include <map>
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
	param_setting(bool is_mandatory,
					bool is_positional,
					std::string return_name,
					std::string short_name,
					std::string long_name,
					std::string description,
					std::string default_value):
		is_mandatory(is_mandatory),
		is_positional(is_positional),
		return_name(return_name),
		short_name(short_name),
		long_name(long_name),
		description(description),
		default_value(default_value) {};
};

size_t find(char *argv[], size_t begin, size_t size, std::string searched_obj)
{
	for(size_t i = begin; i < size; i++)
		if(argv[i] == searched_obj)
			return i;
	return size;
}

std::map<std::string, std::string> parse_param(std::vector<param_setting> settings, int argc, char *argv[])
{
	std::map<std::string, std::string> parsed_params;
	bool are_still_positional = true;
	size_t i = 0;
	for(auto it = settings.begin(); it != settings.end(); it++)
	{
		if(!are_still_positional && (*it).is_positional)
			std::cerr << "Error : all positional arguments should be at the beginning of the setting" << std::endl;
			throw "Error : all positional arguments should be at the beginning of the setting";
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
				std::cerr << "Error: Missing required positional parameter" << std::endl;
				throw "Error: Missing required positional parameter";
		}
		else
		{
			size_t pos = find(argv, i, argc, (*it).short_name);
			pos++;
			if (pos>=argc)
			{
				size_t pos = find(argv, i, argc, (*it).long_name);
				pos++;
			}
			if(pos<argc)
				parsed_params[(*it).return_name] = argv[pos];
			else if(!(*it).is_mandatory)
				parsed_params[(*it).return_name] = (*it).default_value;
			else
				std::cerr << "Error: Missing required non-positional parameter" << std::endl;
				throw "Error: Missing required non-positional parameter";
		}
	}
	return parsed_params;
}
#endif // PARAMPARSER_H_INCLUDED