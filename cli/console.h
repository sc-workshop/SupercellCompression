#pragma once
#include <string>
#include <algorithm>

#define OptionPrefix "--"

void make_uppercase(std::string& data);
void make_lowercase(std::string& data);
std::string get_option(int argc, char* argv[], const char* option);
bool get_bool_option(int argc, char* argv[], const char* option);
int get_int_option(int argc, char* argv[], const char* option);
bool is_option_in(int argc, char* argv[], const char* option);
