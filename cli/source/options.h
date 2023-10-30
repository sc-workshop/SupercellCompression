#pragma once
#include <filesystem>
namespace fs = std::filesystem;

#include "console.h"
#include <iostream>
#include <thread>

enum class CompressionMethod
{
	LZMA = 0,
	ZSTD,
	LZHAM,
};

enum class CompressionHeader
{
	None = 0,
	LZHAM,
	SC,
};

enum class InputFormat
{
	Binary = 0,
};

enum class Operations
{
	Unknown = 0,

	Compress,
	Decompress
};

// Helper class to parse options from command line
struct CommandLineOptions
{
	CommandLineOptions(int argc, char* argv[]);

	Operations operation = Operations::Unknown;

	fs::path input_path;
	fs::path output_path;

	// Basic options
	CompressionMethod method = CompressionMethod::LZMA;
	CompressionHeader header = CompressionHeader::SC;
	InputFormat input = InputFormat::Binary;
	unsigned int threads = std::thread::hardware_concurrency();

	// SC props
	bool printMetadata = false;

	// Lzma compression prop
	bool use_long_unpacked_length = false;
};