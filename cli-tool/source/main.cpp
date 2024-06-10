#include "core/console/console.h"

#include "core/io/file_stream.h"

#include "compression/decompressor.h"
#include "compression/compressor.h"

#include "compression/signature.h"

#include <algorithm>
#include <cctype>

#include <iostream>

namespace
{
	// Split one string into array of strings by space
	std::vector<std::string> split_into_vector(const std::string& input)
	{
		std::vector<std::string> result;

		int start = 0;
		int end = 0;
		while ((start = input.find_first_not_of(' ', end)) != std::string::npos)
		{
			end = input.find(' ', start);
			result.push_back(input.substr(start, end - start));
		}

		return result;
	}
}

int main(int argc, const char** argv)
{
	// Arguments
	sc::ArgumentParser parser("supercell-compression-cli", "Tool for compress and decompress files using Supercell compression methods");

	parser.add_argument("-d", "--decomrpess", "Decompress file(s)", false);
	parser.add_argument("-c", "--comrpess", "Compress file(s)", false);
	parser.add_argument("-m", "--method", "Sets compression method for \"-c\"/\"--compress\" operation. Possible values - lzma, lzham, zstd (default)", false);

	parser.enable_help();

	auto err = parser.parse(argc, argv);
	if (err)
	{
		std::cout << "Failed to parse command line arguments! Error: " << err.what() << std::endl;
		return -1;
	}

	if (parser.exists("help"))
		parser.print_help();

	// Decompression
	if (parser.exists("d"))
	{
		std::string raw_files = parser.get<std::string>("d");
		std::vector<std::string> files_to_decompress = split_into_vector(raw_files);

		sc::Decompressor decompressor;

		for (const auto& file : files_to_decompress)
		{
			try
			{
				sc::InputFileStream compressed_file(file);
				sc::OutputFileStream decompressed_file(file + "-dec");

				std::cout << "Decompressing " << file << "..." << std::endl;

				decompressor.decompress(compressed_file, decompressed_file);
			}
			catch (sc::Exception& exc)
			{
				std::cout << "Error! " << exc.what() << std::endl;
			}
		}
	}

	// Compression
	if (parser.exists("c"))
	{
		sc::Signature signature = sc::Signature::Zstandard;

		if (parser.exists("m"))
		{
			std::string raw_methods = parser.get<std::string>("m");
			std::vector<std::string> methods = split_into_vector(raw_methods);

			if (methods.size() > 0)
			{
				if (methods.size() > 1)
					std::cout << "Warning! More than 1 compression methods defined! Only the first specified argument will be used!" << std::endl;

				// Get first string argument
				std::string method = methods[0];
				std::transform(method.begin(), method.end(), method.begin(), [](unsigned char c) {return std::tolower(c); }); // to lowercase

				if (method == "lzma")
				{
					signature = sc::Signature::Lzma;
					std::cout << "Using LZMA compression method." << std::endl;
				}
				else if (method == "lzham")
				{
					signature = sc::Signature::Lzham;
					std::cout << "Using LZHAM compression method." << std::endl;
				}
				else if (method == "zstd")
				{
					signature = sc::Signature::Zstandard;
					std::cout << "Using Zstandard compression method." << std::endl;
				}
				else
				{
					std::cout << "Unknown compression method!" << std::endl;
				}
			}
			else
			{
				std::cout << "No compression methods is specified!" << std::endl;
			}
		}

		std::cout << "Using default compression method." << std::endl;

		std::string raw_files = parser.get<std::string>("c");
		std::vector<std::string> files_to_compress = split_into_vector(raw_files);

		sc::Compressor compressor;

		sc::Compressor::Context context;
		context.signature = signature;

		for (const auto& file : files_to_compress)
		{
			try
			{
				sc::InputFileStream compressed_file(file);
				sc::OutputFileStream decompressed_file(file + "-comp");

				std::cout << "Compressing " << file << "..." << std::endl;

				compressor.compress(compressed_file, decompressed_file, context);
			}
			catch (sc::Exception& exc)
			{
				std::cout << "Error! " << exc.what() << std::endl;
			}
		}
	}

	return 0;
}
