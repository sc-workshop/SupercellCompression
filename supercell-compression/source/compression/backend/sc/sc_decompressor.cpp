#include "compression/backend/sc/sc_decompressor.h"

#include "compression/backend/sc/signature.h"

#include "compression/backend/lzma/lzma_decompressor.h"
#include "compression/backend/lzham/lzham_decompressor.h"
#include "compression/backend/zstd/zstd_decompressor.h"

#include <core/exception/exception.h>
#include <core/io/memory_stream.h>

#include <vector>

using namespace wk;

namespace sc
{
	namespace compression
	{
		namespace flash
		{
			void Decompressor::ReadLzmaCompressedData(wk::Stream& input, wk::Stream& output)
			{
				std::uint8_t header[lzma::PROPS_SIZE];
				input.read(header, lzma::PROPS_SIZE);
				std::uint32_t unpacked_length = input.read_unsigned_int();
				LzmaDecompressor context(header, unpacked_length);
				context.decompress(input, output);
			}

			std::optional<flexbuffers::Reference> Decompressor::decompress(Stream& input, Stream& output)
			{
				// Reading file magic
				std::uint16_t magic = input.read_unsigned_short(Endian::Big);
				if (magic != SC_MAGIC)
				{
					// Backward compatibility to old files without SC magic
					if (LzmaDecompressor::ValidStream(input)) {
						input.seek(input.position() - sizeof(std::uint16_t));
						Decompressor::ReadLzmaCompressedData(input, output);
						return std::nullopt;
					}
					else {
						throw Exception("Bad compressed file magic!");
					}
				}

				std::optional<flexbuffers::Reference> result = std::nullopt;

				// Checking compression version
				std::uint32_t version = input.read_unsigned_int(Endian::Big);
				if (version == 4)
				{
					version = input.read_unsigned_int(Endian::Big);

					uint32_t* data_end = (uint32_t*)((uint8_t*)input.data() + input.length());
					uint32_t length = swap_endian(*--data_end);
					const uint8_t* data = (uint8_t*)data_end - length;

					bool valid = flexbuffers::VerifyBuffer(data, length);
					if (valid)
					{
						result = flexbuffers::GetRoot(data, length);
					}
					
				}

				// Read hash
				std::uint32_t hash_length = input.read_unsigned_int(Endian::Big);

				MemoryStream hash(hash_length);
				input.read(hash.data(), hash_length);

				// Version 3 is zstandard
				if (version == 3)
				{
					ZstdDecompressor context;
					context.decompress(input, output);
				}
				else if (version == 1) // Version 1 is lzma or lzham
				{
					// Check SCLZ magic
					if (input.read_unsigned_int() == SC_LZHAM_MAGIC)
					{
						LzhamDecompressor::Props props;
						props.dict_size_log2 = input.read_unsigned_byte();
						props.unpacked_length = input.read_unsigned_int();

						LzhamDecompressor context(props);
						context.decompress(input, output);
					}
					else
					{
						input.seek(input.position() - sizeof(uint32_t));
						ReadLzmaCompressedData(input, output);
					}
				}
				else if (version == 2) // Version 2 is uncompressed data // DaniilSV: are you sure?
				{
					output.write(input.data(), input.length() - input.position());
				}

				return result;
			}
		}
	}
}