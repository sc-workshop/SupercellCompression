#include "main.h"
#include "backend/interface.h"

#include "backend/sc.hpp"

#include <core/memory/ref.h>

using namespace sc::compression;
using namespace wk;

namespace
{
	Mode string_to_mode(std::string mode)
	{
		if (mode == "compress")
		{
			return Mode::Compress;
		}
		else if (mode == "decompress")
		{
			return Mode::Decompress;
		}

		throw Exception("Wrong mode!");
	}

	Container string_to_container(std::string container)
	{
		if (container == "none")
		{
			return Container::None;
		}
		else if (container == "sc")
		{
			return Container::SC;
		}

		throw Exception("Wrong container!");
	}

	Method string_to_method(std::string method)
	{
		if (method == "lzma")
		{
			return Method::LZMA;
		}
		else if (method == "lzham")
		{
			return Method::LZHAM;
		}
		else if (method == "zstd")
		{
			return Method::ZSTD;
		}

		throw Exception("Wrong method!");
	}
}

namespace
{
	fs::path get_unique_name(fs::path basename, Mode mode, Method method, Container container)
	{
		//if (container == Container::SC)
		//{
		std::string prefix = "";
		switch (mode)
		{
		case Mode::Decompress:
			prefix = "d-";
			break;

		case Mode::Compress:
			prefix = "c-";
			break;

		default:
			break;
		}

		return fs::path(prefix).concat(basename.u16string());
		//}
	}

	Ref<CommandLineInterface> InterfaceFactrory(Method method, Container container)
	{
		if (container == Container::SC)
		{
			return CreateRef<ScCommandLineInterface>();
		}

		throw Exception("Failed to create interface!");
	}
}

void supercell_compression_cli(ArgumentParser& parser)
{
	Mode mode = string_to_mode(parser.get<std::string>("mode"));
	Method method = string_to_method(parser.get<std::string>("--method"));
	Container container = string_to_container(parser.get<std::string>("--container"));

	std::vector<std::string> input_paths = parser.get<std::vector<std::string>>("--file");

	bool custom_output = parser.get<std::string>("--output").empty() == false;
	fs::path output = "";
	if (custom_output)
	{
		output = fs::path(parser.get<std::string>("--output"));
		if (output.is_relative())
		{
			output = fs::absolute(output);
		}
		if (!fs::exists(output) || !fs::is_directory(output))
		{
			fs::create_directory(output);
		}
	}

	auto processor = InterfaceFactrory(method, container);
	processor->parse(parser);

	OperationContext context;
	for (size_t i = 0; input_paths.size() > i; i++)
	{
		fs::path input_path = input_paths[i];

		if (fs::is_directory(input_path))
		{
			continue;
		}

		if (input_path.has_relative_path())
		{
			input_path = fs::absolute(input_path);
		}

		fs::path basename = custom_output ? input_path.filename() : get_unique_name(input_path.filename(), mode, method, container);
		fs::path basedir = custom_output ? output : input_path.parent_path();
		fs::path output_path = basedir / basename;

		try
		{
			InputFileStream input_file(input_path);
			OutputFileStream output_file(output_path);

			context.current_file = input_path;
			context.current_output_file = output_path;
			context.current_index = i;

			switch (mode)
			{
			case Mode::Decompress:
				processor->decompress(context, input_file, output_file);
				std::cout << input_path.make_preferred() << " decompressed to " << output_path.make_preferred() << std::endl;
				break;
			case Mode::Compress:
				processor->compress(context, input_file, output_file, method);
				std::cout << input_path.make_preferred() << " compressed to " << output_path.make_preferred() << std::endl;
				break;
			default:
				break;
			}
		}
		catch (Exception& exception)
		{
			std::cout << "Failed: " << input_path.make_preferred() << std::endl;
			std::cout << "Reason: " << exception.what() << std::endl;
		}
	}
}

int main(int argc, const char** argv)
{
	fs::path executable = argv[0];
	fs::path executable_name = executable.stem();

	// Arguments
	ArgumentParser parser(executable_name.string(), "Tool for compress and decompress files using Supercell compression methods");

	parser.add_argument("mode")
		.help("Possible values: compress, decompress")
		.choices("compress", "decompress");

	parser.add_argument("-i", "--file")
		.help("Input files")
		.append();

	parser.add_argument("-c", "--container")
		.help(" Sets container type for compression. Possible values: none, sc")
		.choices("none", "sc")
		.default_value("sc");

	parser.add_argument("-m", "--method")
		.help("Sets compression method for compress operation. Possible values: lzma, lzham, zstd")
		.choices("lzma", "lzham", "zstd")
		.default_value("zstd");

	parser.add_argument("-o", "--output")
		.help("save output to folder instead of changing extension or adding prefix")
		.default_value("");

	ScCommandLineInterface::initialize(parser);

	try {
		parser.parse_args(argc, argv);
	}
	catch (const std::exception& err) {
		std::cout << parser << std::endl;

		std::cout << "Error! " << err.what() << std::endl;
		return 1;
	}

	if (parser["--help"] == true || argc == 1)
	{
		std::cout << parser << std::endl;
		return 0;
	}

	try
	{
		supercell_compression_cli(parser);
	}
	catch (const Exception& exception)
	{
		std::cout << "Unknown unexpected exception: " << exception.what() << std::endl;
	}

	return 0;
}