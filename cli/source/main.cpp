#include "main.h"

#include "io/buffer_stream.h"
#include "io/file_stream.h"
#include "exception/GeneralRuntimeException.h"
#include "stb/stb.h"

void print_usage()
{
	print("> Usage: {Operation} {input_file} {output_file} {...options}");
	print("> Example: c file.bin file_compressed.bin --container=SC --method=zstd");

	std::cout << std::endl;
	print("> Operations: ");
	print("> d, decompress: Decompress binary file");
	print("> c, compress: Compress binary file");
	print("> v, convert: Converts a file from one file type to another of the same format");
	std::cout << std::endl;

	print("> Additional options: ");

	print("General:");
	print("   " OptionPrefix"container: Sets container type for compression. Possible values - None, SC. Default - None");
	print("   " OptionPrefix"format: Defines behavior for some compression or converting modes. Possible values - Binary, Image. Default - Binary");

	std::cout << std::endl;

	print("Binary Options:");
	print("   " OptionPrefix"method: Sets compression method by which file will be compressed or decompressed. Possible values - LZMA, ZSTD, LZHAM, ASTC. Default - ZSTD");

	std::cout << std::endl;

	print("Image Options:");
	print("   " OptionPrefix"imageVerticalFlip: Flips image when saving in jpg, png and similar image formats");
	print("   " OptionPrefix"imageSaveMips: Saves texture mip maps if they are supported and exist");

	std::cout << std::endl;

	print("SC:");
	print("   " OptionPrefix"print_sc_metadata: If file has metadata, it will be displayed in the console. Boolean option.");

	std::cout << std::endl;

	print("LZMA:");
	print("   " OptionPrefix"lzmaLongUnpackedLength: Writes length of decompressed data in classic long bytes. Boolean option.");

	std::cout << std::endl;
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

int main(int argc, char* argv[])
{
	printf("Ultimate SC Compression Tool - %s Command Line app - Compiled %s %s\n\n", PLATFORM, __DATE__, __TIME__);
	if (argc < 4) {
		print_usage();
		return 0;
	}

	CommandLineOptions options(argc, argv);

	// -- Files --
	if (options.input_path.empty() || !fs::exists(options.input_path)) {
		std::cout << "[ERROR] Input file does not exist." << std::endl;
		return 0;
	}

	if (options.output_path.empty()) {
		std::cout << "[ERROR] Output file path is empty" << std::endl;
		return 0;
	}

	try
	{
		// -- Timer --
		time_point start_time = high_resolution_clock::now();

		std::string operation_describe = "";

		if (options.operation == Operations::Compress)
		{
			operation_describe = "Compress";
			print("Compressing...");

			sc::InputFileStream input_stream(options.input_path);
			sc::BufferStream output_stream;

			if (!binary_compressing(input_stream, output_stream, options))
			{
				return 1;
			}

			// Writing memory to file
			{
				sc::OutputFileStream output_file(options.output_path);
				output_file.write(output_stream.data(), output_stream.length());
			}
		}
		else if (options.operation == Operations::Decompress)
		{
			operation_describe = "Decompress";

			sc::BufferStream input_stream;
			sc::OutputFileStream output_stream(options.output_path);

			// Loading file to memory
			{
				sc::InputFileStream input_file(options.input_path);
				input_stream.resize(input_file.length());
				input_file.read(input_stream.data(), input_file.length());
			}

			print("Decompressing...");

			if (!binary_decompressing(input_stream, output_stream, options))
			{
				return 1;
			}
		}
		else if (options.operation == Operations::Convert)
		{
			operation_describe = "Convert";

			sc::BufferStream input_stream;

			// Loading file to memory
			{
				sc::InputFileStream input_file(options.input_path);
				input_stream.resize(input_file.length());
				input_file.read(input_stream.data(), input_file.length());
			}

			print("Converting...");

			bool result = false;

			switch (options.file_format)
			{
			case FileFormat::Image:
				result = image_convert(input_stream, options);
				break;
			default:
				print("[ERROR] Selected file format is not supported in convert option. Supported formats: Image");
				break;
			}

			if (!result)
			{
				return 1;
			}

			// Writing memory to file
			// {
			// 	sc::OutputFileStream output_file(options.output_path);
			// 	output_file.write(output_stream.data(), output_stream.length());
			// }
		}
		else
		{
			std::cout << "[ERROR] Unknown operation" << std::endl;
			return 0;
		}

		std::cout << operation_describe << " operation took: ";
		print_time(start_time);
	}
	catch (const sc::GeneralRuntimeException& exception)
	{
		std::cout << "[ERROR] " << exception.what() << std::endl;
		return 1;
	}

	return 0;
}