#include "options.h"

CommandLineOptions::CommandLineOptions(int argc, char* argv[])
{
#pragma region Basic Settings
	{
		std::string param = get_option(argc, argv, OptionPrefix "format");
		if (!param.empty())
		{
			make_lowercase(param);

			if (param == "binary")
			{
				file_format = FileFormat::Binary;
			}
			else if (param == "image")
			{
				file_format = FileFormat::Image;
			}
			else
			{
				std::cout << "[WARNING] An unknown type of file format is specified. Instead, default is used - Binary" << std::endl;
			}
		}
	}

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

		if (operation_name == "v" || operation_name == "convert")
		{
			operation = Operations::Convert;
		}
	}

	input_path = fs::path(argv[2]);
	output_path = fs::path(argv[3]);

	if (is_option_in(argc, argv, OptionPrefix "threads"))
	{
		threads = get_int_option(argc, argv, OptionPrefix "threads");
	}
#pragma endregion

#pragma region Binary Settings
	{
		std::string container_name = get_option(argc, argv, OptionPrefix "container");
		if (!container_name.empty())
		{
			make_lowercase(container_name);

			if (container_name == "none")
			{
				binary.container = FileContainer::None;
			}
			else if (container_name == "sc")
			{
				binary.container = FileContainer::SC;
			}
			else if (container_name == "astc")
			{
				binary.container = FileContainer::ASTC;
			}
			else
			{
				std::cout << "[WARNING] An unknown type of container is specified. Instead, default is used - None" << std::endl;
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
				binary.method = CompressionMethod::LZMA;
			}
			else if (method_name == "zstd")
			{
				binary.method = CompressionMethod::ZSTD;
			}
			else if (method_name == "lzham")
			{
				binary.method = CompressionMethod::LZHAM;
			}
			else if (method_name == "astc")
			{
				binary.method = CompressionMethod::ASTC;
			}
			else
			{
				std::cout << "[WARNING] An unknown type of compression is specified. Instead, default is used - LZMA" << std::endl;
			}
		}
	}
#pragma endregion

#pragma region Image Settings
	image.flip_images = is_option_in(argc, argv, OptionPrefix "imageVerticalFlip");
	image.save_mip_maps = is_option_in(argc, argv, OptionPrefix "imageSaveMips");

#pragma endregion

#pragma region SC Props
	binary.sc.print_metadata = is_option_in(argc, argv, OptionPrefix "print_sc_metadata");
#pragma endregion

#pragma region LZMA Props
	binary.lzma.use_long_unpacked_length = is_option_in(argc, argv, OptionPrefix "lzmaLongUnpackedLength");

#pragma endregion
}