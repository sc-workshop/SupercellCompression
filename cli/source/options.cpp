#include "options.h"

CommandLineOptions::CommandLineOptions(int argc, char* argv[])
{
	{
		std::string operation_name = std::string(argv[1]);
		make_lowercase(operation_name);

		if (operation_name == "c" || operation_name == "compress")
		{
			operation = Operations::Compress;
		}

		if (operation_name == "d" || operation_name == "decompress")
		{
			operation = Operations::Decompress;
		}
	}

	input_path = fs::path(argv[2]);
	output_path = fs::path(argv[3]);

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