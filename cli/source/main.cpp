#include "main.h"

#include "io/buffer_stream.h"
#include "io/file_stream.h"
#include "exception/GeneralRuntimeException.h"

void print_usage()
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
	printf("SC Compression V2.0 - %s Command Line app - Compiled %s %s\n\n", PLATFORM, __DATE__, __TIME__);
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

	if (options.operation == Operations::Compress)
	{
		// -- Timer --
		time_point start_time = high_resolution_clock::now();

		sc::InputFileStream input_stream(options.input_path);
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