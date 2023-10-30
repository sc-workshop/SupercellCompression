#include "main.h"
#include "SupercellCompression.h"

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

void ZSTD_decompress(sc::Stream& input, sc::Stream& output, CommandLineOptions options)
{
	sc::Decompressor::Zstd context;
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