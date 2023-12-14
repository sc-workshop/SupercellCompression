#include "console.h"

void make_lowercase(std::string& data)
{
	std::transform(data.begin(), data.end(), data.begin(),
		[](unsigned char c) {
			return static_cast<unsigned char>(std::tolower(c));
		});
}

void make_uppercase(std::string& data)
{
	std::transform(data.begin(), data.end(), data.begin(),
		[](unsigned char c) {
			return static_cast<unsigned char>(std::toupper(c));
		});
}

std::string get_option(int argc, char* argv[], const char* option)
{
	std::string cmd;
	for (int i = 0; i < argc; ++i)
	{
		std::string arg = argv[i];
		if (0 == arg.find(option))
		{
			std::size_t found = arg.find_first_of("=");
			cmd = arg.substr(found + 1);
			return cmd;
		}
	}
	return cmd;
}

bool get_bool_option(int argc, char* argv[], const char* option)
{
	std::string value = get_option(argc, argv, option);
	if (value.empty()) return false;

	if (value == "1" || value == "y" || value == "yes")
	{
		return true;
	}

	return false;
}

int get_int_option(int argc, char* argv[], const char* option)
{
	std::string value = get_option(argc, argv, option);
	if (value.empty()) return 0;

	return std::stoi(value);
}

bool is_option_in(int argc, char* argv[], const char* option) {
	for (int i = 0; i < argc; ++i)
	{
		std::string arg = argv[i];
		if (0 == arg.find(option))
		{
			return true;
		}
	}
	return false;
}