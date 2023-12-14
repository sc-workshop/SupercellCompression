#include "SupercellCompression/ScCompression.h"

#include "io/memory_stream.h"
#include "exception/io/BinariesExceptions.h"

namespace sc {
	namespace ScCompression
	{
		namespace Decompressor
		{
			void read_metadata(uint8_t* buffer_end, MetadataAssetArray& metadataArray)
			{
				uint32_t asset_total_count = 0;
				bool unknown_bool = false;

				uint8_t* metadata_header = buffer_end - 6;

				// -- Strings and Hashes data --
				uint32_t strings_info_field_size = 0;
				uint8_t hash_info_field_size = 0;

				uint8_t* asset_info_ptr = nullptr;
				uint8_t* strings_ptr = nullptr;

				char info_field_size = 0;
				char metadata_flags = *(char*)metadata_header;
				if ((metadata_flags & 0xFC) == 0x24)
				{
					// -- Initial Part --
					uint8_t asset_info_field_size = *(metadata_header + 1);

					uint8_t* asset_info_offset_ptr = metadata_header - asset_info_field_size;

					uint32_t asset_info_offset;
					if (asset_info_field_size > 3)
					{
						asset_info_offset = *(uint32_t*)asset_info_offset_ptr;
					}
					else if (asset_info_field_size <= 1)
					{
						asset_info_offset = *asset_info_offset_ptr;
					}
					else
					{
						asset_info_offset = *(uint16_t*)asset_info_offset_ptr;
					}

					// -- Second Part --
					asset_info_ptr = asset_info_offset_ptr - asset_info_offset;

					char strings_bits_offset = metadata_flags & 3;
					info_field_size = 1ULL << (metadata_flags & 3);
					if (info_field_size >= 3u)
					{
						int32_t strings_data_offset = 0xFFFFFFFDULL << strings_bits_offset;
						if (info_field_size >= 8u)
						{
							uint8_t* string_data_ptr = asset_info_ptr + strings_data_offset;

							strings_info_field_size = 8;
							hash_info_field_size = 8;

							strings_ptr = &asset_info_ptr[strings_data_offset - *(uint32_t*)&asset_info_ptr[strings_data_offset]];
							strings_info_field_size = *(char*)(string_data_ptr[info_field_size]);
						}
						else
						{
							hash_info_field_size = 1ULL << (metadata_flags & 3);

							strings_info_field_size = *(uint32_t*)&asset_info_ptr[strings_data_offset + info_field_size];

							uint32_t strings_array_offset = *(uint32_t*)(asset_info_ptr + strings_data_offset);

							int strings_offset = strings_data_offset - strings_array_offset;
							strings_ptr = asset_info_ptr + strings_offset;
						}

						asset_total_count = *(uint32_t*)(asset_info_ptr - info_field_size);
						unknown_bool = false;
					}
					else if (info_field_size > 1u)
					{
						hash_info_field_size = 1ULL << (metadata_flags & 3);

						int bit_offset = 0xFFFFFFFD << strings_bits_offset;
						uint8_t* strings_array_data_offset = &asset_info_ptr[bit_offset];

						strings_ptr = &strings_array_data_offset[-*(uint16_t*)strings_array_data_offset];

						strings_info_field_size = strings_array_data_offset[info_field_size];

						asset_total_count = *(uint16_t*)(asset_info_ptr - info_field_size);
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
					uint32_t strings_total_count;
					uint32_t hash_total_count;

					if (strings_info_field_size > 3)
					{
						strings_total_count = *(uint32_t*)(strings_ptr - strings_info_field_size);
					}
					else if (strings_info_field_size > 1)
					{
						strings_total_count = *(uint16_t*)(strings_ptr - strings_info_field_size);
					}
					else
					{
						strings_total_count = *(uint8_t*)(strings_ptr - strings_info_field_size);
					}

					if (unknown_bool)
					{
						if (hash_info_field_size > 1u)
							hash_total_count = *(uint16_t*)&asset_info_ptr[-info_field_size];
						else
							hash_total_count = asset_info_ptr[-info_field_size];
					}
					else
					{
						hash_total_count = *(uint32_t*)&asset_info_ptr[-info_field_size];
					}

					uint8_t* hash_flags_ptr = &asset_info_ptr[hash_total_count * info_field_size];

					for (uint32_t i = 0; asset_total_count > i; i++)
					{
						MetadataAsset asset;

						uint8_t* string_offset_ptr = strings_ptr + i * strings_info_field_size;
						int string_offset = 0;

						if (strings_total_count > i)
						{
							if (strings_info_field_size > 3)
							{
								string_offset = *(uint32_t*)string_offset_ptr;
							}
							else if (strings_info_field_size <= 1)
							{
								string_offset = *string_offset_ptr;
							}
							else
							{
								string_offset = *(uint16_t*)string_offset_ptr;
							}

							asset.name = std::string((const char*)&string_offset_ptr[-string_offset]);
						}

						if (hash_total_count > i)
						{
							uint8_t hash_flag = hash_flags_ptr[i];
							bool is_valid_hash = hash_flag >> 2 == 0x19;
							if (hash_flag >> 2 != 0x19)
								is_valid_hash = hash_flag >> 2 == 5;

							if (is_valid_hash)
							{
								uint8_t* hash_offset_ptr = &asset_info_ptr[i * info_field_size];

								int32_t hash_offset;
								if (hash_info_field_size > 3u)
								{
									hash_offset = *(uint32_t*)hash_offset_ptr;
								}
								else if (hash_info_field_size <= 1u) {
									hash_offset = *hash_offset_ptr;
								}
								else
								{
									hash_offset = *(uint16_t*)hash_offset_ptr;
								}

								uint8_t hash_length_field_size = 1ULL << (hash_flag & 3);

								uint8_t* hash_ptr = &hash_offset_ptr[-hash_offset];
								uint32_t hash_size = 0;
								if (hash_length_field_size > 3u)
								{
									hash_size = *(uint32_t*)&hash_ptr[-hash_length_field_size];
								}
								else if (hash_length_field_size > 1u)
								{
									hash_size = *(uint16_t*)&hash_ptr[-hash_length_field_size];
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

			void decompress(Stream& input, Stream& output, MetadataAssetArray* metadataArray)
			{
				using namespace sc::Decompressor;

				int16_t magic = input.read_unsigned_short(Endian::Big);
				if (magic != SC_MAGIC)
				{
					throw BadMagicException((uint8_t*)&SC_MAGIC, (uint8_t*)&magic, sizeof(uint16_t));
				}

				uint8_t* compressed_data_ptr = nullptr;
				size_t compressed_data_length = input.length();

				uint32_t version = input.read_unsigned_int(Endian::Big);
				if (version == 4)
				{
					version = input.read_unsigned_int(Endian::Big);

					uint8_t* buffer_end = (uint8_t*)input.data() + input.length();

					uint32_t chunk_length = swap_endian(*(uint32_t*)(buffer_end - 4));

					// metadata_length + metadata_length_bytes_size + START length
					compressed_data_length -= chunk_length;
					compressed_data_length -= 4 + 5;

					if (metadataArray)
					{
						read_metadata(buffer_end, *metadataArray);
					}
				}

				uint32_t hash_length = input.read_unsigned_int(Endian::Big);

				std::vector<uint8_t> hash(hash_length);
				input.read(hash.data(), hash_length);

				compressed_data_ptr = (uint8_t*)input.data() + input.position();
				compressed_data_length -= input.position();

				if (version == 3)
				{
					MemoryStream compressed_data(compressed_data_ptr, compressed_data_length);
					Zstd context;
					context.decompress_stream(compressed_data, output);
				}
				else if (version == 1)
				{
					// If SCLZ
					if (*(uint32_t*)compressed_data_ptr == 0x5A4C4353)
					{
						MemoryStream compressed_data(compressed_data_ptr + 4, compressed_data_length - 4);
						Lzham::Props props;
						props.dict_size_log2 = compressed_data.read_unsigned_int();
						props.unpacked_length = compressed_data.read_unsigned_int();
						sc::Decompressor::Lzham context(props);
						context.decompress_stream(compressed_data, output);
					}
					else
					{
						MemoryStream compressed_data(compressed_data_ptr, compressed_data_length);

						uint8_t header[lzma::PROPS_SIZE];
						compressed_data.read(header, lzma::PROPS_SIZE);

						uint32_t unpacked_length = compressed_data.read_unsigned_int();

						Lzma context(header, unpacked_length);
						context.decompress_stream(compressed_data, output);
					}
				}
			}
		}
	}
}