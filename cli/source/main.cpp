#include "main.h"

#include "io/buffer_stream.h"
#include "io/file_stream.h"
#include "exception/GeneralRuntimeException.h"

#define print(text) std::cout << text << std::endl

void print_usage()
{
	print("> Usage: {Operation} {input_file} {output_file} {...options}");
	print("> Example: c file.bin file_compressed.bin --container=SC --method=zstd");

	std::cout << std::endl;
	print("> Operations: ");
	print("> d, decompress: Decompress file");
	print("> c, compress: Compress file");
	print("> v, convert: Convert file from one data type to other");
	std::cout << std::endl;

	print("> Additional options: ");

	print("General:");
	print("		" OptionPrefix"container: Sets container type for compression. Possible values - None, SC. Default - None");
	print("		" OptionPrefix"method: Sets compression method by which file will be compressed. Possible values - LZMA, ZSTD, LZHAM, ASTC. Default - ZSTD");
	print("		" OptionPrefix"format: Defines behavior for some compression modes. Possible values - Binary, Image. Default - Binary");
	print("		" "Option notes: ");
	print("		" "> When using image format and ASTC decompression, file will be saved as a PNG image or other available format depending on extension. Otherwise, image buffer will be saved as is");

	print("SC:");
	print("		" OptionPrefix"print_metadata: If file has metadata, it will be displayed in the console. Boolean option.");

	print("LZMA:");
	print("		" OptionPrefix"longUnpackedLength: Writes length of decompressed data in classic long bytes. Boolean option.");
}

void print_time(time_point<high_resolution_clock> start, time_point<high_resolution_clock> end)
{
	milliseconds msTime = duration_cast<milliseconds>(end - start);
	seconds secTime = duration_cast<seconds>(msTime);
	minutes minTime = duration_cast<minutes>(secTime);

	if (minTime.count() > 0)
	{
		std::cout << minTime.count() << " minutes, ";
		msTime -= duration_cast<milliseconds>(minTime);
		secTime -= duration_cast<seconds>(minTime);
	}

	if (secTime.count() > 0)
	{
		std::cout << secTime.count() << " seconds, ";
		msTime -= duration_cast<milliseconds>(secTime);
	}

	std::cout << msTime.count() << " miliseconds";
}

bool binary_compressing(sc::Stream& input_stream, sc::Stream& output_stream, CommandLineOptions& options)
{
	switch (options.binary.method)
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
		std::cout << "[ERROR] Unknown compression method" << std::endl;
		return false;
	}

	return true;
}

bool binary_decompressing(sc::Stream& input_stream, sc::Stream& output_stream, CommandLineOptions& options)
{
	if (options.binary.container != FileContainer::None)
	{
		switch (options.binary.container)
		{
		case FileContainer::SC:
			SC_decompress(input_stream, output_stream, options);
			break;

		default:
			std::cout << "[ERROR] Unknown compression container" << std::endl;
			return false;
		}
	}
	else
	{
		switch (options.binary.method)
		{
		case CompressionMethod::LZMA:
			LZMA_decompress(input_stream, output_stream, options);
			break;

		case CompressionMethod::ZSTD:
			ZSTD_decompress(input_stream, output_stream, options);
			break;

		case CompressionMethod::ASTC:
			ASTC_decompress(input_stream, output_stream, options);
			break;

		case CompressionMethod::LZHAM:
			LZHAM_decompress(input_stream, output_stream, options);
			break;

		default:
			std::cout << "[ERROR] Unknown compression method" << std::endl;
			return false;
		}
	}

	return true;
}

int main(int argc, char* argv[])
{
	printf("Ultimate SC Compression Tool - %s Command Line app - Compiled %s %s\n\n", PLATFORM, __DATE__, __TIME__);
	if (argc < 4) {
		print_usage();
		return 0;
	}

	CommandLineOptions options(argc, argv);

	// -- Files --
	if (options.input_path.empty()) {
		std::cout << "[ERROR] Input file does not exist." << std::endl;
		return 0;
	}

	if (options.output_path.empty()) {
		std::cout << "[ERROR] Output file path is empty" << std::endl;
		return 0;
	}

	try
	{
		if (options.operation == Operations::Compress)
		{
			// -- Timer --
			time_point start_time = high_resolution_clock::now();

			sc::InputFileStream input_stream(options.input_path);
			sc::BufferStream output_stream;

			std::cout << "Compressing..." << std::endl;

			if (!binary_compressing(input_stream, output_stream, options))
			{
				return 1;
			}

			// Writing memory to file
			{
				sc::OutputFileStream output_file(options.output_path);
				output_file.write(output_stream.data(), output_stream.length());
			}

			std::cout << "Compress operation took: ";
			print_time(start_time);
		}
		else if (options.operation == Operations::Decompress)
		{
			// -- Timer --
			time_point start_time = high_resolution_clock::now();

			sc::BufferStream input_stream;
			sc::OutputFileStream output_stream(options.output_path);

			// Loading file to memory
			{
				sc::InputFileStream input_file(options.input_path);
				input_stream.resize(input_file.length());
				input_file.read(input_stream.data(), input_file.length());
			}

			std::cout << "Decompressing..." << std::endl;

			if (!binary_decompressing(input_stream, output_stream, options))
			{
				return 1;
			}

			std::cout << "Decompress operation took: ";
			print_time(start_time);
		}
		else
		{
			std::cout << "[ERROR] Unknown operation" << std::endl;
			return 0;
		}
	}
	catch (const sc::GeneralRuntimeException& exception)
	{
		std::cout << "[ERROR] " << exception.what() << std::endl;
		return 1;
	}

	return 0;
}