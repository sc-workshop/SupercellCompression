#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include <filesystem>
namespace fs = std::filesystem;

#include <chrono>
using namespace std::chrono;

#include "SupercellCompression.h"

#include "io/buffer_stream.h"
#include "io/file_stream.h"
#include "exception/GeneralRuntimeException.h"

#if defined _WIN32
#define PLATFORM "Windows"
#elif defined __linux
#define PLATFORM "Linux"
#elif defined __macosx
#define PLATFORM "MacOSX"
#else
#define PLATFORM "Unknown"
#endif

#define OptionPrefix "--"

void make_lowercase(std::string& data)
{
	std::transform(data.begin(), data.end(), data.begin(),
		[](unsigned char c) { return std::tolower(c); });
}

void make_uppercase(std::string& data)
{
	std::transform(data.begin(), data.end(), data.begin(),
		[](unsigned char c) { return std::toupper(c); });
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

// Helper class to parse options from command line
struct CommandLineOptions
{
	CommandLineOptions(int argc, char* argv[])
	{
		{
			std::string header_name = get_option(argc, argv, OptionPrefix "header");
			if (!header_name.empty())
			{
				make_lowercase(header_name);

				if (header_name == "none")
				{
					header = CompressionHeader::None;
				}
				else if (header_name == "sc")
				{
					header = CompressionHeader::SC;
				}
				else if (header_name == "lzham")
				{
					header = CompressionHeader::LZHAM;
				}
				else
				{
					std::cout << "[WARNING] An unknown type of header is specified. Instead, default is used - SC" << std::endl;
				}
			}
		}

		{
			std::string method_name = get_option(argc, argv, OptionPrefix "method");
			if (!method_name.empty())
			{
				make_lowercase(method_name);

				if (method_name == "lzma")
				{
					method = CompressionMethod::LZMA;
				}
				else if (method_name == "zstd")
				{
					method = CompressionMethod::ZSTD;
				}
				else if (method_name == "lzham")
				{
					method = CompressionMethod::LZHAM;
				}
				else
				{
					std::cout << "[WARNING] An unknown type of compression is specified. Instead, default is used - LZMA" << std::endl;
				}
			}
		}

		use_long_unpacked_length = get_bool_option(argc, argv, OptionPrefix "longUnpackedLength");

		if (is_option_in(argc, argv, OptionPrefix "threads"))
		{
			threads = get_int_option(argc, argv, OptionPrefix "threads");
		}

		printMetadata = is_option_in(argc, argv, OptionPrefix "printMetadata");
	}

	// Basic options
	CompressionMethod method = CompressionMethod::LZMA;
	CompressionHeader header = CompressionHeader::SC;
	unsigned int threads = std::thread::hardware_concurrency();

	// SC props
	bool printMetadata = false;

	// Lzma compression prop
	bool use_long_unpacked_length = false;
};

void printUsage()
{
	std::cout << "> Usage: {Operation} {input_file} {output_file} {...options}" << std::endl;
	std::cout << "> Example: c file.bin file_compressed.bin --header=SC --method=zstd" << std::endl;

	std::cout << std::endl;
	std::cout << "> Operations: " << std::endl;
	std::cout << "> d, decompress: Decompress file" << std::endl;
	std::cout << "> c, compress: Compress file" << std::endl;
	std::cout << std::endl;

	std::cout << "> Additional options: " << std::endl;

	std::cout << "	" << "General:" << std::endl;
	std::cout << "		" << OptionPrefix"header: Sets header type for compression. Possible values - None, SC, LZHAM. Default - SC" << std::endl;
	std::cout << "		" << OptionPrefix"method: Sets compression method by which file will be compressed. Possible values - LZMA, ZSTD, LZHAM. Default - LZMA" << std::endl;

	std::cout << "	" << "SC:" << std::endl;
	std::cout << "		" << OptionPrefix"printMetadata: If file has metadata, it will be displayed in the console. Boolean option." << std::endl;

	std::cout << "	" << "LZMA:" << std::endl;
	std::cout << "		" << OptionPrefix"longUnpackedLength: Writes length of decompressed data in classic long bytes. Boolean option." << std::endl;
}

void print_time(time_point<high_resolution_clock> start, time_point<high_resolution_clock> end = high_resolution_clock::now()) {
	milliseconds msTime = duration_cast<milliseconds>(end - start);
	if (msTime.count() < 1000) {
		std::cout << msTime.count() << " miliseconds";
	}
	else {
		seconds secTime = duration_cast<seconds>(msTime);

		std::cout << secTime.count() << " seconds, ";

		std::cout << (msTime - duration_cast<milliseconds>(secTime)).count() << " miliseconds" << std::endl;
	}
}

void LZMA_compress(sc::Stream& input, sc::Stream& output, CommandLineOptions options)
{
	switch (options.header)
	{
	case CompressionHeader::SC:
	{
		using namespace sc::ScCompression;

		Compressor::CompressorContext context;
		context.signature = Signature::Lzma;
		context.threads_count = options.threads;

		Compressor::compress(input, output, context);
	}
	break;

	case CompressionHeader::None:
	{
		sc::Compressor::LzmaProps props;
		props.level = 6;
		props.pb = 2;
		props.lc = 3;
		props.lp = 0;
		props.use_long_unpacked_length = options.use_long_unpacked_length;
		props.numThreads = options.threads;

		sc::Compressor::Lzma context(props);
		context.compress_stream(input, output);
	}
	break;
	default:
		std::cout << "[ERROR] Unsupported header for LZMA. Supported only None and SC." << std::endl;
		break;
	}
}

void LZMA_decompress(sc::Stream& input, sc::Stream& output, CommandLineOptions options)
{
	uint8_t header[LZMA_PROPS_SIZE];
	input.read(&header, LZMA_PROPS_SIZE);

	uint64_t unpacked_length;
	if (options.use_long_unpacked_length)
	{
		unpacked_length = input.read_unsigned_long();
	}
	else
	{
		unpacked_length = input.read_unsigned_int();
	}

	sc::Decompressor::Lzma context(header, unpacked_length);
	context.decompress_stream(input, output);
}

void ZSTD_compress(sc::Stream& input, sc::Stream& output, CommandLineOptions options)
{
	switch (options.header)
	{
	case CompressionHeader::SC:
	{
		using namespace sc::ScCompression;

		Compressor::CompressorContext context;
		context.signature = Signature::Zstandard;
		context.threads_count = options.threads;

		Compressor::compress(input, output, context);
	}
	break;

	case CompressionHeader::None:
	{
		sc::Compressor::ZstdProps props;

		sc::Compressor::Zstd context(props);
		context.compress_stream(input, output);
	}
	break;
	default:
		std::cout << "[ERROR] Unsupported header for ZSTD. Supported only None and SC." << std::endl;
		break;
	}
}

void ZSTD_decompress(sc::Stream& input, sc::Stream& output, CommandLineOptions options)
{
	sc::Decompressor::Zstd context;
	context.decompress_stream(input, output);
}

void LZHAM_compress(sc::Stream& input, sc::Stream& output, CommandLineOptions options)
{
	sc::Compressor::LzhamCompressProps props;
	props.m_dict_size_log2 = 18;

	switch (options.header)
	{
	case CompressionHeader::SC:
	{
		using namespace sc::ScCompression;

		Compressor::CompressorContext context;
		context.signature = Signature::Lzham;
		context.threads_count = options.threads;

		Compressor::compress(input, output, context);
	}
	break;

	case CompressionHeader::LZHAM:
	{
		output.write_int('0HZL');
		output.write_unsigned_int(props.m_dict_size_log2);
		output.write_unsigned_long(input.length());
	}
	case CompressionHeader::None:
	{
		sc::Compressor::Lzham context(props);
		context.compress_stream(input, output);
	}
	break;
	default:
		std::cout << "[ERROR] Unsupported header for LZHAM. Supported only None, SC and LZHAM" << std::endl;
		break;
	}
}

void LZHAM_decompress(sc::Stream& input, sc::Stream& output, CommandLineOptions options)
{
	uint32_t magic = input.read_int();
	if (magic != '0HZL')
	{
		std::cout << "[Error] LZHAM file has bad magic";
	}

	sc::Decompressor::LzhamDecompressProps props;
	props.m_dict_size_log2 = input.read_unsigned_int();
	props.m_unpacked_length = input.read_unsigned_long();

	sc::Decompressor::Lzham context(props);
	context.decompress_stream(input, output);
}

void SC_decompress(sc::Stream& input, sc::Stream& output, CommandLineOptions options)
{
	using namespace sc::ScCompression;

	if (options.printMetadata)
	{
		MetadataAssetArray array;
		Decompressor::decompress(input, output, &array);

		if (array.size())
		{
			std::cout << "Metadata list: " << std::endl;

			for (MetadataAsset& asset : array)
			{
				std::cout << "Name: " << asset.name << ", Hash: ";
				for (char& byte : asset.hash)
				{
					std::cout << std::setfill('0') << std::setw(2) << std::hex << (0xff & (unsigned int)byte);
				}

				std::cout << std::endl;
			}
		}
	}
	else
	{
		Decompressor::decompress(input, output, nullptr);
	}
}

int main(int argc, char* argv[])
{
	printf("SC Compression V2.0 - %s Command Line app - Compiled %s %s\n\n", PLATFORM, __DATE__, __TIME__);
	if (argc < 4) {
		printUsage();
		return 0;
	}

	std::string operation(argv[1]);
	make_lowercase(operation);

	// -- Files --
	fs::path input_path(argv[2] ? argv[2] : "");
	if (input_path.empty() || !fs::exists(input_path)) {
		std::cout << "[ERROR] Input file does not exist." << std::endl;
		return 0;
	}

	fs::path output_path(argv[3]);
	if (output_path.empty()) {
		std::cout << "[ERROR] Output file is empty" << std::endl;
		return 0;
	}

	CommandLineOptions options(argc, argv);

	if (operation == "c" || operation == "compress")
	{
		// -- Timer --
		time_point start_time = high_resolution_clock::now();

		//sc::BufferStream input_stream;
		sc::InputFileStream input_stream(input_path);
		sc::BufferStream output_stream;

		std::cout << "Compressing..." << std::endl;

		try
		{
			switch (options.method)
			{
			case CompressionMethod::LZMA:
				LZMA_compress(input_stream, output_stream, options);
				break;

			case CompressionMethod::ZSTD:
				ZSTD_compress(input_stream, output_stream, options);
				break;

			case CompressionMethod::LZHAM:
				LZHAM_compress(input_stream, output_stream, options);
				break;

			default:
				break;
			}
		}
		catch (const sc::GeneralRuntimeException& exception)
		{
			std::cout << "[ERROR] " << exception.what() << std::endl;
			return 1;
		}

		// Writing memory to file
		{
			sc::OutputFileStream output_file(output_path);
			output_file.write(output_stream.data(), output_stream.length());
		}

		std::cout << "Compress operation took: ";
		print_time(start_time);
	}
	else if (operation == "d" || operation == "decompress")
	{
		// -- Timer --
		time_point start_time = high_resolution_clock::now();

		sc::BufferStream input_stream;
		sc::OutputFileStream output_stream(output_path);

		// Loading file to memory
		{
			sc::InputFileStream input_file(input_path);
			input_stream.resize(input_file.length());
			input_file.read(input_stream.data(), input_file.length());
		}

		std::cout << "Decompressing..." << std::endl;

		if (options.header != CompressionHeader::None)
		{
			switch (options.header)
			{
			case CompressionHeader::LZHAM:
				LZHAM_decompress(input_stream, output_stream, options);
				break;
			case CompressionHeader::SC:
				SC_decompress(input_stream, output_stream, options);
				break;
			default:
				break;
			}
		}
		else
		{
			try
			{
				switch (options.method)
				{
				case CompressionMethod::LZMA:
					LZMA_decompress(input_stream, output_stream, options);
					break;

				case CompressionMethod::ZSTD:
					ZSTD_decompress(input_stream, output_stream, options);
					break;

				default:
					break;
				}
			}
			catch (const sc::GeneralRuntimeException& exception)
			{
				std::cout << "[ERROR] " << exception.what() << std::endl;
				return 1;
			}
		}

		std::cout << "Decompress operation took: ";
		print_time(start_time);
	}
	else
	{
		std::cout << "[ERROR] Unknown operation" << std::endl;
		return 0;
	}

	return 0;
}