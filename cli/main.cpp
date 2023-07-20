#include <fstream>
#include <iostream>
#include <string>
#include <chrono>

#include <SupercellBytestream.h>
#include <SupercellCompression.h>

#include <filesystem>

namespace fs = std::filesystem;

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

std::string GetOption(int argc, char* argv[], const std::string& option)
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

bool OptionIn(int argc, char* argv[], const std::string& option) {
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

void PrintResult(sc::CompressorResult result) {
	if (result == sc::CompressorResult::COMPRESSION_SUCCES) {
		std::cout << "[INFO] Succes!" << std::endl;
		return;
	}

	std::cout << "[ERROR] ";

	switch (result)
	{
	case sc::CompressorResult::ALLOC_ERROR:
		std::cout << "Failed to allocate memory for compress" << std::endl;
		break;
	case sc::CompressorResult::LZHAM_STREAM_INIT_ERROR:
		std::cout << "Failed to initialize LZHAM stream" << std::endl;
		break;
	case sc::CompressorResult::LZHAM_CORRUPTED_DATA_ERROR:
		std::cout << "Failed to compress corrupted LZHAM data" << std::endl;
		break;
	case sc::CompressorResult::LZMA_STREAM_INIT_ERROR:
		std::cout << "Failed to initialize LZMA stream" << std::endl;
		break;
	case sc::CompressorResult::ZSTD_STREAM_INIT_ERROR:
		std::cout << "Failed to initialize ZSTD stream" << std::endl;
		break;
	case sc::CompressorResult::ZSTD_CORRUPTED_DATA_ERROR:
		std::cout << "Failed to compress corrupted ZSTD data" << std::endl;
		break;
	default:
		break;
	}
}

void PrintResult(sc::DecompressorResult result) {
	if (result == sc::DecompressorResult::DECOMPRESSION_SUCCES) {
		std::cout << "[INFO] Succes!" << std::endl;
		return;
	}

	std::cout << "[ERROR] ";

	switch (result)
	{
	case sc::DecompressorResult::ALLOC_ERROR:
		std::cout << "Failed to allocate memory for decompress" << std::endl;
		break;
	case sc::DecompressorResult::CORRUPTED_HEADER_ERROR:
		std::cout << "File has corrupted header" << std::endl;
		break;
	case sc::DecompressorResult::STREAM_ERROR:
		std::cout << "Failed to get data from stream" << std::endl;
		break;
	case sc::DecompressorResult::LZHAM_CORRUPTED_DICT_SIZE_ERROR:
		std::cout << "LZHAM data has bad dictionary size" << std::endl;
		break;
	case sc::DecompressorResult::LZHAM_STREAM_INIT_ERROR:
		std::cout << "Failed to initialize LZMA decompress stream" << std::endl;
		break;
	case sc::DecompressorResult::LZHAM_CORRUPTED_DATA_ERROR:
		std::cout << "LZHAM data is corrupted" << std::endl;
		break;
	case sc::DecompressorResult::LZMA_CORRUPTED_DATA_ERROR:
		std::cout << "LZMA data is corrupted" << std::endl;
		break;
	case sc::DecompressorResult::LZMA_MISSING_END_MARKER_ERROR:
		std::cout << "LZMA data does not have an end marker" << std::endl;
		break;
	case sc::DecompressorResult::ZSTD_CORRUPTED_DATA_ERROR:
		std::cout << "ZSTD data is corrupted" << std::endl;
		break;
	case sc::DecompressorResult::ZSTD_STREAM_INIT_ERROR:
		std::cout << "Failed to initialize ZSTD decompress stream" << std::endl;
		break;
	default:
		break;
	}
}

void printUsage() {
	printf("Usage: [mode] input output options\n");

	printf("\n");

	printf("Modes:\n");
	printf("c - Compress file.\n");
	printf("d - Decompress file.\n");

	printf("\n");

	printf("Options:\n");
	printf("-m - Compression mode: LZMA, LZHAM, ZSTD. Default: ZSTD\n");
	printf("-t - Thread count. Default: All CPU cores\n");

	printf("\n");

	printf("Flags:\n");
	printf("--common - Process file as common file (like compressed .csv)\n");

	printf("\n");

	printf("Example: c file.sc file_compressed.sc -m=ZSTD -t=2\n");
}

void PrintTime(time_point<high_resolution_clock> start, time_point<high_resolution_clock> end) {
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

int main(int argc, char* argv[])
{
	printf("SC Compression - %s Command Line app - Compiled %s %s\n\n", PLATFORM, __DATE__, __TIME__);
	if (argc <= 1) {
		printUsage();
		return 0;
	}

	// Files
	std::string mode(argv[1]);

	fs::path inFilepath(argv[2] ? argv[2] : "");
	if (inFilepath.empty() || !fs::exists(inFilepath)) {
		std::cout << "[ERROR] Input file does not exist." << std::endl;
		return 0;
	}

	fs::path outFilepath(argv[3]);
	if (outFilepath.empty()) {
		std::cout << "[ERROR] Output file does not exist." << std::endl;
		return 0;
	}

	// Flags
	bool isCommon = OptionIn(argc, argv, "--common");

	// Timer
	time_point startTime = high_resolution_clock::now();

	// Modes
	if (mode == "d") {
		sc::ReadFileStream inStream(inFilepath);
		sc::WriteFileStream outStream(outFilepath);

		try {
			if (isCommon) {
				sc::Decompressor::CommonDecompress(inStream, outStream);
			}
			else {
				sc::Decompressor::Decompress(inStream, outStream);
			}
		}
		catch (const std::exception& err) {
			std::cout << "[ERROR] " << err.what() << std::endl;
		}

		inStream.close();
		outStream.close();
	}
	else if (mode == "c") {
		sc::CompressorContext context;

		std::string signatureArg = GetOption(argc, argv, "-m=");
		std::string theardArg = GetOption(argc, argv, "-t=");

		if (signatureArg == "ZSTD")
		{
			context.signature = sc::CompressionSignature::ZSTD;
		}
		else if (signatureArg == "LZMA")
		{
			context.signature = sc::CompressionSignature::LZMA;
		}
		else if (signatureArg == "LZHAM")
		{
			context.signature = sc::CompressionSignature::LZHAM;
		}

		if (theardArg.size() > 0) {
			context.threadCount = std::stoi(theardArg);
		}

		sc::ReadFileStream inStream(inFilepath);
		sc::WriteFileStream outStream(outFilepath);

		sc::CompressorResult result;

		if (isCommon) {
			result = sc::Compressor::CommonCompress(inStream, outStream, context);
		}
		else {
			result = sc::Compressor::Compress(inStream, outStream, context);
		}

		PrintResult(result);
	}
	else {
		printf("[ERROR] Unknown mode.");
		return 0;
	}

	// Result

	time_point endTime = high_resolution_clock::now();
	std::cout << "[INFO] Operation took: ";

	PrintTime(startTime, endTime);

	return 0;
}