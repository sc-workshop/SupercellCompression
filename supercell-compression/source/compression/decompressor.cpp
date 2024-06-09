#include "compression/decompressor.h"

#include "compression/signature.h"

#include "compression/backend/lzma/lzma_decompressor.h"
#include "compression/backend/lzham/lzham_decompressor.h"
#include "compression/backend/zstd/zstd_decompressor.h"

#include <core/exception/exception.h>
#include <core/io/memory_stream.h>

namespace sc
{
	// TODO(pavidloq): make it more... pretty?
	void read_metadata(std::uint8_t* buffer_end, MetadataAssetArray& metadataArray)
	{
		std::uint32_t asset_total_count = 0;
		bool unknown_bool = false;

		std::uint8_t* metadata_header = buffer_end - 6;

		// -- Strings and Hashes data --
		std::uint32_t strings_info_field_size = 0;
		std::uint8_t hash_info_field_size = 0;

		std::uint8_t* asset_info_ptr = nullptr;
		std::uint8_t* strings_ptr = nullptr;

		char info_field_size = 0;
		char metadata_flags = *(char*)metadata_header;
		if ((metadata_flags & 0xFC) == 0x24)
		{
			// -- Initial Part --
			std::uint8_t asset_info_field_size = *(metadata_header + 1);

			std::uint8_t* asset_info_offset_ptr = metadata_header - asset_info_field_size;

			std::uint32_t asset_info_offset;
			if (asset_info_field_size > 3)
			{
				asset_info_offset = *(std::uint32_t*)asset_info_offset_ptr;
			}
			else if (asset_info_field_size <= 1)
			{
				asset_info_offset = *asset_info_offset_ptr;
			}
			else
			{
				asset_info_offset = *(std::uint16_t*)asset_info_offset_ptr;
			}

			// -- Second Part --
			asset_info_ptr = asset_info_offset_ptr - asset_info_offset;

			char strings_bits_offset = metadata_flags & 3;
			info_field_size = 1ULL << (metadata_flags & 3);
			if (info_field_size >= 3u)
			{
				std::int32_t strings_data_offset = 0xFFFFFFFDULL << strings_bits_offset;
				if (info_field_size >= 8u)
				{
					std::uint8_t* string_data_ptr = asset_info_ptr + strings_data_offset;

					strings_info_field_size = 8;
					hash_info_field_size = 8;

					strings_ptr = &asset_info_ptr[strings_data_offset - *(std::uint32_t*)&asset_info_ptr[strings_data_offset]];
					strings_info_field_size = *(std::uint8_t*)(string_data_ptr + info_field_size);
				}
				else
				{
					hash_info_field_size = 1ULL << (metadata_flags & 3);

					strings_info_field_size = *(std::uint32_t*)&asset_info_ptr[strings_data_offset + info_field_size];

					std::uint32_t strings_array_offset = *(std::uint32_t*)(asset_info_ptr + strings_data_offset);

					int strings_offset = strings_data_offset - strings_array_offset;
					strings_ptr = asset_info_ptr + strings_offset;
				}

				asset_total_count = *(std::uint32_t*)(asset_info_ptr - info_field_size);
				unknown_bool = false;
			}
			else if (info_field_size > 1u)
			{
				hash_info_field_size = 1ULL << (metadata_flags & 3);

				int bit_offset = 0xFFFFFFFD << strings_bits_offset;
				std::uint8_t* strings_array_data_offset = &asset_info_ptr[bit_offset];

				strings_ptr = &strings_array_data_offset[-*(std::uint16_t*)strings_array_data_offset];

				strings_info_field_size = *(std::uint16_t*)(strings_array_data_offset + info_field_size);

				asset_total_count = *(std::uint16_t*)(asset_info_ptr - info_field_size);
				unknown_bool = true;
			}
			else
			{
				strings_info_field_size = asset_info_ptr[-2];

				strings_ptr = &asset_info_ptr[-3 - asset_info_ptr[-3]];

				asset_total_count = asset_info_ptr[-info_field_size];

				strings_info_field_size = 1;
				hash_info_field_size = 1;
			}
		}
		else
		{
			return;
		}

		// -- Last part --
		if (asset_total_count)
		{
			// Here we get number of strings and hashes. For some reason, in theory, there can be a different number of them.
			std::uint32_t strings_total_count;
			std::uint32_t hash_total_count;

			if (strings_info_field_size > 3)
			{
				strings_total_count = *(std::uint32_t*)(strings_ptr - strings_info_field_size);
			}
			else if (strings_info_field_size > 1)
			{
				strings_total_count = *(std::uint16_t*)(strings_ptr - strings_info_field_size);
			}
			else
			{
				strings_total_count = *(std::uint8_t*)(strings_ptr - strings_info_field_size);
			}

			if (unknown_bool)
			{
				if (hash_info_field_size > 1u)
					hash_total_count = *(std::uint16_t*)&asset_info_ptr[-info_field_size];
				else
					hash_total_count = asset_info_ptr[-info_field_size];
			}
			else
			{
				hash_total_count = *(std::uint32_t*)&asset_info_ptr[-info_field_size];
			}

			std::uint8_t* hash_flags_ptr = &asset_info_ptr[hash_total_count * info_field_size];

			for (std::uint32_t i = 0; asset_total_count > i; i++)
			{
				MetadataAsset asset;

				std::uint8_t* string_offset_ptr = strings_ptr + i * strings_info_field_size;
				int string_offset = 0;

				if (strings_total_count > i)
				{
					if (strings_info_field_size > 3)
					{
						string_offset = *(std::uint32_t*)string_offset_ptr;
					}
					else if (strings_info_field_size <= 1)
					{
						string_offset = *string_offset_ptr;
					}
					else
					{
						string_offset = *(std::uint16_t*)string_offset_ptr;
					}

					asset.name = std::string((const char*)&string_offset_ptr[-string_offset]);
				}

				if (hash_total_count > i)
				{
					std::uint8_t hash_flag = hash_flags_ptr[i];
					bool is_valid_hash = hash_flag >> 2 == 0x19;
					if (hash_flag >> 2 != 0x19)
						is_valid_hash = hash_flag >> 2 == 5;

					if (is_valid_hash)
					{
						std::uint8_t* hash_offset_ptr = &asset_info_ptr[i * info_field_size];

						std::int32_t hash_offset;
						if (hash_info_field_size > 3u)
						{
							hash_offset = *(std::uint32_t*)hash_offset_ptr;
						}
						else if (hash_info_field_size <= 1u) {
							hash_offset = *hash_offset_ptr;
						}
						else
						{
							hash_offset = *(std::uint16_t*)hash_offset_ptr;
						}

						std::uint8_t hash_length_field_size = 1ULL << (hash_flag & 3);

						std::uint8_t* hash_ptr = &hash_offset_ptr[-hash_offset];
						std::uint32_t hash_size = 0;
						if (hash_length_field_size > 3u)
						{
							hash_size = *(std::uint32_t*)&hash_ptr[-hash_length_field_size];
						}
						else if (hash_length_field_size > 1u)
						{
							hash_size = *(std::uint16_t*)&hash_ptr[-hash_length_field_size];
						}
						else
						{
							hash_size = hash_ptr[-hash_length_field_size];
						}

						asset.hash = std::vector<char>((char*)hash_ptr, (char*)hash_ptr + hash_size);
					}
				}

				metadataArray.push_back(asset);
			}
		}
	}

	void Decompressor::decompress(Stream& input, Stream& output, MetadataAssetArray* metadata)
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

			// metadata_length + metadata_length_bytes_size + START length
			compressed_data_length -= chunk_length;
			compressed_data_length -= 4 + 5;

			// Read metadata
			if (metadata)
			{
				read_metadata(buffer_end, *metadata);
			}
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
