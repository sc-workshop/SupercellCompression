#pragma once
#include "options.h"
#include "io/stream.h"
#include "io/file_stream.h"

#include <chrono>
using namespace std::chrono;

#define print(text) std::cout << text << std::endl

#if defined _WIN32
#define PLATFORM "Windows"
#elif defined __linux
#define PLATFORM "Linux"
#elif defined __macosx
#define PLATFORM "MacOSX"
#else
#define PLATFORM "Unknown"
#endif

// Main
void print_usage();
void print_time(time_point<high_resolution_clock> start, time_point<high_resolution_clock> end = high_resolution_clock::now());

bool binary_compressing(sc::Stream& input_stream, sc::Stream& output_stream, CommandLineOptions& options);
bool binary_decompressing(sc::Stream& input_stream, sc::Stream& output_stream, CommandLineOptions& options);
bool image_convert(sc::Stream& input_stream, CommandLineOptions& options);

int main(int argc, char* argv[]);
