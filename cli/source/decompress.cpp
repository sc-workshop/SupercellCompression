#include "main.h"
#include "SupercellCompression.h"

void LZMA_decompress(sc::Stream& input, sc::Stream& output, CommandLineOptions& options)
{
	uint8_t header[LZMA_PROPS_SIZE];
	input.read(&header, LZMA_PROPS_SIZE);

	uint64_t unpacked_length;
	if (options.binary.lzma.use_long_unpacked_length)
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

void LZHAM_decompress(sc::Stream& input, sc::Stream& output, CommandLineOptions& options)
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

void ZSTD_decompress(sc::Stream& input, sc::Stream& output, CommandLineOptions& options)
{
	sc::Decompressor::Zstd context;
	context.decompress_stream(input, output);
}

void ASTC_decompress(sc::Stream& input, sc::Stream& output, CommandLineOptions& options)
{
	using namespace sc::Decompressor;
	using namespace sc;

	uint16_t width;
	uint16_t height;
	AstcDecompressProps props;

	Astc::read_header(input, width, height, props.blocks_x, props.blocks_y);

	Astc context(props);
	context.decompress_image(width, height, sc::Image::BasePixelType::RGBA, input, output);
}

void SC_decompress(sc::Stream& input, sc::Stream& output, CommandLineOptions& options)
{
	using namespace sc::ScCompression;

	if (options.binary.sc.print_metadata)
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