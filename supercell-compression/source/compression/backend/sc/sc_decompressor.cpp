#include "sc_decompressor.h"

#include "signature.h"

#include "compression/backend/lzma/lzma_decompressor.h"
#include "compression/backend/lzham/lzham_decompressor.h"
#include "compression/backend/zstd/zstd_decompressor.h"

#include <core/exception/exception.h>
#include <core/io/memory_stream.h>

namespace sc
{
	namespace compression
	{
		namespace flash
		{
			void Decompressor::decompress(Stream& input, Stream& output, sc::MemoryStream** metadata)
			{
				// Reading file magic
				std::uint16_t magic = input.read_unsigned_short(Endian::Big);
				if (magic != SC_MAGIC)
				{
					throw Exception("Bad compressed file magic!");
				}

				std::size_t compressed_data_length = input.length();

				// Checking compression version
				std::uint32_t version = input.read_unsigned_int(Endian::Big);
				if (version == 4)
				{
					version = input.read_unsigned_int(Endian::Big);

					size_t offset = input.position();

					input.seek(input.length() - 4);
					uint32_t metadata_size = input.read_unsigned_int(Endian::Big);

					if (metadata != nullptr)
					{
						input.seek(input.position() - metadata_size - 4);
						*metadata = new sc::MemoryStream(metadata_size);
						input.read((*metadata)->data(), metadata_size);
					}

					input.seek(offset);

					compressed_data_length -= metadata_size; // Metadata
					compressed_data_length -= 4; // Metadata Length
					compressed_data_length -= 5; // START
				}

				// Read hash
				std::uint32_t hash_length = input.read_unsigned_int(Endian::Big);

				sc::MemoryStream hash(hash_length);
				input.read(hash.data(), hash_length);

				// Version 3 is zstandard
				if (version == 3)
				{
					ZstdDecompressor context;
					context.decompress(input, output);
				}
				else if (version == 1) // Version 1 is lzma or lzham
				{
					std::uint8_t header[lzma::PROPS_SIZE];
					input.read(header, lzma::PROPS_SIZE);
					std::uint32_t unpacked_length = input.read_unsigned_int();

					// Check SCLZ magic
					if (*(std::uint32_t*)header == SC_LZHAM_MAGIC)
					{
						LzhamDecompressor::Props props;
						props.dict_size_log2 = header[4];
						props.unpacked_length = unpacked_length;

						LzhamDecompressor context(props);
						context.decompress(input, output);
					}
					else
					{
						LzmaDecompressor context(header, unpacked_length);
						context.decompress(input, output);
					}
				}
				else if (version == 2) // Version 2 is uncompressed data // DaniilSV: are you sure?
				{
					output.write(input.data(), compressed_data_length);
				}
			}
		}
	}
}