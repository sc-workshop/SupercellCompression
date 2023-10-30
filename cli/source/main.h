#pragma once
#include "options.h"
#include "io/stream.h"

#include <chrono>
using namespace std::chrono;

#if defined _WIN32
#define PLATFORM "Windows"
#elif defined __linux
#define PLATFORM "Linux"
#elif defined __macosx
#define PLATFORM "MacOSX"
#else
#define PLATFORM "Unknown"
#endif

// Compression
void LZMA_compress(sc::Stream& input, sc::Stream& output, CommandLineOptions& options);
void LZMA_decompress(sc::Stream& input, sc::Stream& output, CommandLineOptions& options);

void ZSTD_compress(sc::Stream& input, sc::Stream& output, CommandLineOptions& options);
void ZSTD_decompress(sc::Stream& input, sc::Stream& output, CommandLineOptions& options);

void LZHAM_compress(sc::Stream& input, sc::Stream& output, CommandLineOptions& options);
void LZHAM_decompress(sc::Stream& input, sc::Stream& output, CommandLineOptions& options);

void ASTC_decompress(sc::Stream& input, sc::Stream& output, CommandLineOptions& options);
void SC_decompress(sc::Stream& input, sc::Stream& output, CommandLineOptions& options);

// Main
void print_usage();
void print_time(time_point<high_resolution_clock> start, time_point<high_resolution_clock> end = high_resolution_clock::now());

bool binary_compressing(sc::Stream& input_stream, sc::Stream& output_stream, CommandLineOptions& options);
bool binary_decompressing(sc::Stream& input_stream, sc::Stream& output_stream, CommandLineOptions& options);

int main(int argc, char* argv[]);
