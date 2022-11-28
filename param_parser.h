/**
 * @file param_parser.h
 *  Defines a class to handle command-line parameters
 */
#ifndef PARAMPARSER_H_INCLUDED
#define PARAMPARSER_H_INCLUDED

/**
 * Number of char spaces between beginning of name and "mandatory" column
 *
 * The sizes of names should be under SEPARATING_NAMES_MANDATORY - 1 to avoid
 * ugly printing
 */
#define SEPARATING_NAMES_MANDATORY 30
/// Number of spaces kept for the "mandatory" column
#define SEPARATING_MANDATORY_DESC 12

/// Value that will be given to boolean argument when it is considered True
#define BOOLEAN_TRUE_VALUE "1"
/// Value that will be given to boolean argument when it is considered False
#define BOOLEAN_FALSE_VALUE "0"

#include <fstream>
#include <vector>
#include <map>
#include <math.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <sstream>

/// Handles the command-line parameters and convert them to usable string
class ParametersParser {
	private :
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

			/// True if no command-line value has been provided
			bool is_value_default;

			/// Value to use in the program
			std::string value;

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
			bool is_boolean;
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
				is_boolean(is_boolean),
				is_value_default(true),
				value(default_value) {};
		};

		/// General help message to display in help
		std::string help_message;

		/// List of settings of the program 
		std::vector<param_setting> settings;

		/// True after call to the function parse_param (values can be read)
		bool are_parameters_parsed;

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

		/**
		 * Display the help text
		 *
		 * @param program_name Name and path to the program, for running instructions
		 */
		void display_help(std::string program_name)
		{
			// ---- Print usage ----
			std::cout << "Usage: " << program_name;
			for(auto it = this->settings.begin(); it != this->settings.end() && (*it).is_positional; it++)
				std::cout << " " << (*it).return_name;
			std::cout << " " << "[OPTIONS]" << std::endl << std::endl;

			// ---- Print general help message ----
			std::cout << this->help_message << std::endl << std::endl;

			// ---- Print positional arguments ----
			std::cout << "Positional arguments:" <<std::endl;
			auto it = this->settings.begin();
			for(; it != this->settings.end() && (*it).is_positional; it++)
				std::cout << "  " << (*it).return_name
						<< std::string(std::max(1, (int)(SEPARATING_NAMES_MANDATORY - (*it).return_name.length())), ' ')
						<< ((*it).is_mandatory
								? "mandatory" + std::string(std::max(1, SEPARATING_MANDATORY_DESC - (int)((std::string)"mandatory").length()), ' ')
								: std::string(std::max(1, SEPARATING_MANDATORY_DESC), ' '))
						<< (*it).description << ((*it).default_value != "" ? " (Default: " + (*it).default_value + ")" : "") << std::endl;
			// ---- Print non-positional arguments ----
			std::cout<<std::endl << "Options:" <<std::endl;
			for(; it != this->settings.end(); it++)
				std::cout << "  -" << (*it).short_name << ", --" << (*it).long_name
						<< std::string(std::max(1, SEPARATING_NAMES_MANDATORY - (int)(*it).short_name.length() - (int)(*it).long_name.length() - (int)((std::string)", ---").length()), ' ')
						<< ((*it).is_mandatory
								? "mandatory" + std::string(std::max(1, SEPARATING_MANDATORY_DESC - (int)((std::string)"mandatory").length()), ' ')
								: std::string(std::max(1, SEPARATING_MANDATORY_DESC), ' '))
						<< (*it).description << ((*it).default_value != "" && !(*it).is_boolean ? " (Default: " + (*it).default_value + ")" : "") << std::endl;
		}

		/**
		 * Fill the parameters with values given as command line parameters
		 *
		 * @param argc Number of provided command-line parameters (including name of the program)
		 * @param argv List of the parameters
		 */ 
		void parse_command_line(int argc, char *argv[])
		{
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
			for(auto it = this->settings.begin(); it != this->settings.end(); it++)
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
						(*it).value = argv[i];
						(*it).is_value_default = false;
						i++;
					}
					// Positional argument expected but no or non-positional argument
					// given
					else if((*it).is_mandatory)
					{
						std::cerr << "Error: Missing required positional parameter" << std::endl;
						throw std::runtime_error("Error: Missing required positional parameter");
					}
				}
				// ---- Parsing non-positional arguments ----
				else
				{
					size_t pos = find(argv, i, argc, "-" + (*it).short_name);
					if (pos>=argc)
					{
						pos = find(argv, i, argc, "--" + (*it).long_name);
					}
					// If found
					if(pos<argc)
					{
						if((*it).is_boolean)
						{
							(*it).value = BOOLEAN_TRUE_VALUE;
							(*it).is_value_default = false;
						}
						else
						{
							pos++;
							(*it).value = argv[pos];
							(*it).is_value_default = false;
						}
					}
					else if(!(*it).is_boolean && (*it).is_mandatory)
					{
						std::cerr << "Error: Missing required non-positional parameter" << std::endl;
						throw std::runtime_error("Error: Missing required non-positional parameter");
					}
				}
			}
		}

		/**
		 * Similar as std getline, but allow to escape delimiter using '\'
		 *
		 * @param current_line_stream The stringstream from which to read
		 * @param parsed Line into which the data will be parsed
		 * @param delimiter The delimiter to split lines
		 * @return False if current_line_stream was already at end of line
		 * @bug If the last char is an escaped delimiter, it won't be properly
		 * escaped. In that case you should add an extra delimiter at the end
		 */
		static bool getline_escape(std::stringstream &current_line_stream, std::string& parsed, char delimiter)
		{
			std::string temp_parsed;
			if(!getline(current_line_stream,temp_parsed, delimiter))
				return false;
			else{
				parsed = temp_parsed;
				for(;!parsed.empty() && parsed.back() == '\\' && getline(current_line_stream,temp_parsed, delimiter);)
				{
					parsed.back() = delimiter;
					parsed += temp_parsed;
				}
				return true;
			}
		}
	public :
		/**
		 * ParametersParser constructor
		 *
		 * @param help_message The general message to display in help
		 */
		ParametersParser(std::string help_message) :
			help_message(help_message),
			settings(),
			are_parameters_parsed(false) {}

		/**
		 * Factory building a ParametersParser from a file
		 *
		 * The file should contain the help_message that can be multi-lines but
		 * should not contain empty lines, then an empty line to separate it
		 * from the rest, and then a csv-like list of line, each representing a
		 * parameter with data of a same line separated by ';'. The data should
		 * be - in that order - the return name, the "is_mandatory" boolean, the
		 * "is_positional" boolean, the short name, the long name, the
		 * the description, the default value and the "is_boolean" boolean. The
		 * last two data can be omitted. The data may contain ';' char but it
		 * should be escaped using '\'. If an escaped ';' is located at the end
		 * of the line, an extra ';' should be added after it.
		 *
		 * @param file_name Path to the file to read
		 * @return The ParametersParser containing the help_message and the
		 *  parameters
		 */
		static ParametersParser from_file(std::string file_name)
		{
			std::fstream config_file(file_name);
			std::string current_line;
			std::string help_message;
			if (config_file.is_open()) {
				for(; std::getline(config_file, current_line) && current_line != "";)
					help_message += current_line + "\n";
				ParametersParser to_return(help_message);
				std::string parsed;
				for(; std::getline(config_file, current_line);)
				{
					std::stringstream current_line_stream(current_line);
					getline_escape(current_line_stream,parsed, ';');
					std::string return_name = parsed;
					getline_escape(current_line_stream,parsed, ';');
					bool is_mandatory = (parsed!="0" && parsed != "false" && parsed !="False" && parsed != "");
					getline_escape(current_line_stream,parsed, ';');
					bool is_positional = (parsed!="0" && parsed != "false" && parsed !="False" && parsed != "");
					getline_escape(current_line_stream,parsed, ';');
					std::string short_name = parsed;
					getline_escape(current_line_stream,parsed, ';');
					std::string long_name = parsed;
					getline_escape(current_line_stream,parsed, ';');
					std::string description = parsed;
					std::string default_value = getline_escape(current_line_stream,parsed, ';') ? parsed : "";
					bool is_boolean = getline_escape(current_line_stream,parsed, ';') ? (parsed!="0" && parsed != "false" && parsed !="False" && parsed != ""): false;
					to_return.add_parameter(is_mandatory, is_positional,
											return_name, short_name, long_name,
											description, default_value, is_boolean);
				}
				return to_return;
			}
			else
				throw std::runtime_error("Error when oppening the parameters config file");
		}

		/**
		 * ParametersParser destructor
		 *
		 * Do nothing
		 */
		~ParametersParser(){}

		/**
		 * Add a new accepted parameter
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
		 * @todo Add the parameter at the right place in the vector
		 * @todo Remove necessity to manually put dashes in parameters names
		 */
		void add_parameter(bool is_mandatory,
							bool is_positional,
							std::string return_name,
							std::string short_name,
							std::string long_name,
							std::string description,
							std::string default_value = "",
							bool is_boolean = false)
		{
			this->settings.push_back(param_setting(is_mandatory,
				is_positional,
				return_name,
				short_name,
				long_name,
				description,
				default_value,
				is_boolean));
		}

		/**
		 * Return the value of the parameter
		 *
		 * ParametersParser::parse_param should have been called before
		 *
		 * @param parameter_name Name of the parameter of which value is needed
		 * @return The value of the parameter to use in the program
		 */
		std::string get_value(std::string parameter_name)
		{
			if(!are_parameters_parsed)
				throw std::runtime_error("Parameters should be parsed before use");
			for(auto it = this->settings.begin(); it != this->settings.end(); it++)
				if((*it).return_name == parameter_name)
					return (*it).value;
			throw std::runtime_error("Unknown parameter \"" + parameter_name + "\"");
		}

		/**
		 * Function parsing the parameters or displaying help
		 *
		 * @param argc Size of the argv list
		 * @param argv List of arguments, splitted by words
		 * @return True if the '--help' argument is found, and hence the help is
		 *  displayed, the parsed_param dict is not filled and the program should stop
		 *  immediately after exiting the function.
		 */
		bool parse_param(int argc, char *argv[])
		{
			size_t pos = find(argv, 0, argc, "--help");
			this->are_parameters_parsed = true;
			// Help argument found
			if(pos < argc)
			{
				// Only print help and quit, don't execute the program itself
				display_help(argv[0]);
				return true;
			}
			parse_command_line(argc, argv);
			return false;
		}
};
#endif // PARAMPARSER_H_INCLUDED
