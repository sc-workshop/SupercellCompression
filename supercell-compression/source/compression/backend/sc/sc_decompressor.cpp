#include "compression/backend/sc/sc_decompressor.h"

#include "compression/backend/sc/signature.h"

#include "compression/backend/lzma/lzma_decompressor.h"
#include "compression/backend/lzham/lzham_decompressor.h"
#include "compression/backend/zstd/zstd_decompressor.h"

#include <core/exception/exception.h>
#include <core/io/memory_stream.h>

#include <vector>

namespace sc
{
	namespace compression
	{
		namespace flash
		{
			void Decompressor::decompress(Stream& input, Stream& output)
			{
				// Reading file magic
				std::uint16_t magic = input.read_unsigned_short(Endian::Big);
				if (magic != SC_MAGIC)
				{
					throw Exception("Bad compressed file magic!");
				}

				std::uint8_t* compressed_data_ptr = nullptr;
				std::size_t compressed_data_length = input.length();

				// Checking compression version
				std::uint32_t version = input.read_unsigned_int(Endian::Big);
				if (version == 4)
				{
					version = input.read_unsigned_int(Endian::Big);

					// Calculate metadata offset
					std::uint8_t* buffer_end = (std::uint8_t*)input.data() + input.length();
					std::uint32_t chunk_length = swap_endian(*(std::uint32_t*)(buffer_end - 4));
				}

				// Read hash
				std::uint32_t hash_length = input.read_unsigned_int(Endian::Big);

				std::vector<std::uint8_t> hash(hash_length);
				input.read(hash.data(), hash_length);

				// Prepare data
				compressed_data_ptr = (std::uint8_t*)input.data() + input.position();
				compressed_data_length -= input.position();

				// Version 3 is zstandard
				if (version == 3)
				{
					MemoryStream compressed_data(compressed_data_ptr, compressed_data_length);

					ZstdDecompressor context;
					context.decompress(compressed_data, output);
				}
				else if (version == 1) // Version 1 is lzma or lzham
				{
					// Check SCLZ magic
					if (*(std::uint32_t*)compressed_data_ptr == SC_LZHAM_MAGIC)
					{
						MemoryStream compressed_data(compressed_data_ptr + 4, compressed_data_length - 4);

						LzhamDecompressor::Props props;
						props.dict_size_log2 = compressed_data.read_unsigned_byte();
						props.unpacked_length = compressed_data.read_unsigned_int();

						LzhamDecompressor context(props);
						context.decompress(compressed_data, output);
					}
					else
					{
						MemoryStream compressed_data(compressed_data_ptr, compressed_data_length);

						std::uint8_t header[lzma::PROPS_SIZE];
						compressed_data.read(header, lzma::PROPS_SIZE);

						std::uint32_t unpacked_length = compressed_data.read_unsigned_int();

						LzmaDecompressor context(header, unpacked_length);
						context.decompress(compressed_data, output);
					}
				}
				else if (version == 2) // Version 2 is uncompressed data
				{
					MemoryStream uncompressed_data(compressed_data_ptr, compressed_data_length);
					output.write(uncompressed_data.data(), compressed_data_length);
				}
			}
		}
	}
}