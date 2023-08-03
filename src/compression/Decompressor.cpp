#include "SupercellCompression/Decompressor.h"

#include <iostream>

#include "caching/cache.h"

#include "backend/LzmaCompression.h"
#include "backend/LzhamCompression.h"
#include "backend/ZstdCompression.h"

using namespace std;

namespace sc {
	namespace Decompressor
	{
		DecompressorResult CommonDecompress(BufferStream& input, Bytestream& output, CompressionSignature signature) {
			switch (signature)
			{
			case CompressionSignature::LZMA:
				return LZMA::Decompress(input, output);
				break;

			case CompressionSignature::LZHAM:
				return LZHAM::Decompress(input, output);
				break;

			case CompressionSignature::ZSTD:
				return ZSTD::Decompress(input, output);
				break;

			default:
				output.write(input.data(), input.size());
				return DecompressorResult::DECOMPRESSION_SUCCES;
			}
		}

		DecompressorResult ReadHeader(BufferStream& input, CompressionSignature& signature, std::vector<uint8_t>& hash, MetadataCallback callback) {
			uint16_t magic = input.readUInt16BE();

			if (magic != 0x5343) {
#ifdef _DEBUG
				signature = sc::CompressionSignature::NONE;
				return DecompressorResult::DECOMPRESSION_SUCCES;
#else
				return DecompressorResult::CORRUPTED_HEADER_ERROR;
#endif
			}

			uint32_t version = input.readUInt32BE();
			if (version == 4) {
				version = input.readUInt32BE();

				if (callback)
				{
					uint8_t* buffer = input.data() + input.size();

					uint8_t* header_ptr = buffer - 6;
					uint8_t* info_ptr = nullptr;

					uint32_t asset_total_count = 0;
					bool unknown_bool = true;

					uint8_t* strings_ptr = nullptr;

					// A lot of flags for bit shifts and much more
					char data_flags = *(char*)header_ptr;

					// Data size for strings and hash
					int32_t strings_info_field_size = 0;
					int32_t hash_info_field_size = 0;

					char info_field_size = 0;

					// Probably a flag for smth like "if file uses metadata"
					if ((data_flags & 0xFC) == 0x24)
					{
						// Here we are trying to get an offset for bytes that contain another offset
						// that points to bytes that indicate the number of assets, strings and hashes

						// Something like a bytes data type index. Why are "dynamic" data types needed here at all?
						uint8_t info_offset_field_size = *(header_ptr + 1);
						header_ptr -= info_offset_field_size;

						// this is a very strange location of these flags. why not just put everything in one place?
						char strings_bits_offset = data_flags & 3;
						info_field_size = 1ULL << (data_flags & 3);

						// Offset to number ofassets, strings, hashes
						uint32_t info_offset;
						if (info_offset_field_size > 3)
						{
							info_offset = *(uint32_t*)header_ptr;
						}
						else if (info_offset_field_size <= 1)
						{
							info_offset = *header_ptr;
						}
						else
						{
							info_offset = *(uint16_t*)header_ptr;
						}

						info_ptr = header_ptr - info_offset;

						// And here we get field size for strings and hashes and also total number of "assets" aka export names
						if (info_field_size > 3u)
						{
							int32_t strings_data_offset = 0xFFFFFFFDULL << strings_bits_offset;
							if (info_field_size >= 8u)
							{
								uint8_t* string_data_ptr = info_ptr + strings_data_offset;

								strings_info_field_size = 8;
								hash_info_field_size = 8;

								strings_ptr = &info_ptr[strings_data_offset - *(uint32_t*)&info_ptr[strings_data_offset]];
								strings_info_field_size = *(char*)(string_data_ptr[1ULL << (data_flags & 3)]);
							}
							else
							{
								hash_info_field_size = 1ULL << (data_flags & 3);
								strings_info_field_size = *(uint32_t*)&info_ptr[strings_data_offset + info_field_size];
								strings_ptr = info_ptr + (strings_data_offset - *(uint32_t*)&info_ptr[strings_data_offset]);
							}

							asset_total_count = *(uint32_t*)(info_ptr - info_field_size);
							unknown_bool = false;
						}
						else if (info_field_size > 1u)
						{
							hash_info_field_size = 1ULL << (data_flags & 3);

							int bit_offset = 0xFFFFFFFD << strings_bits_offset;
							uint8_t* strings_array_data_offset = &info_ptr[bit_offset];

							strings_ptr = &strings_array_data_offset[-*(uint16_t*)strings_array_data_offset];

							strings_info_field_size = strings_array_data_offset[info_field_size];

							asset_total_count = *(uint16_t*)(info_ptr - info_field_size);
							unknown_bool = true;
						}
						else
						{
							strings_info_field_size = info_ptr[-2];

							strings_ptr = &info_ptr[-3 - info_ptr[-3]];

							asset_total_count = info_ptr[-info_field_size];

							strings_info_field_size = 1;
							hash_info_field_size = 1;
						}
					}

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
								hash_total_count = *(uint16_t*)&info_ptr[-info_field_size];
							else
								hash_total_count = info_ptr[-info_field_size];
						}
						else
						{
							hash_total_count = *(uint32_t*)&info_ptr[-info_field_size];
						}

						uint8_t* hash_flags_ptr = &info_ptr[hash_total_count * info_field_size];

						for (uint32_t i = 0; asset_total_count > i; i++)
						{
							char* name_string = "\0";
							uint8_t* hash_ptr = nullptr;
							uint32_t hash_size = 0;

							uint8_t* string_offset_ptr = strings_ptr + i * (uint8_t)strings_info_field_size;
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

								name_string = (char*)&string_offset_ptr[-string_offset];
							}

							if (hash_total_count > i)
							{
								uint8_t hash_flag = hash_flags_ptr[i];
								bool is_valid_hash = hash_flag >> 2 == 0x19;
								if (hash_flag >> 2 != 0x19)
									is_valid_hash = hash_flag >> 2 == 5;

								if (is_valid_hash)
								{
									uint8_t* hash_offset_ptr = &info_ptr[i * info_field_size];

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

									hash_ptr = &hash_offset_ptr[-hash_offset];
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
								}
							}

							callback(name_string, (char*)hash_ptr, hash_size);
						}
					}
				}
			}

			if (version == 3) {
				signature = CompressionSignature::ZSTD;
			}

			uint32_t hashSize = input.readUInt32BE();
			hash = std::vector<uint8_t>(hashSize);
			input.read(hash.data(), hashSize);

			if (version == 1)
			{
				uint32_t compressMagic = input.readUInt32();

				// SCLZ
				if (compressMagic == 0x5A4C4353)
				{
					signature = CompressionSignature::LZHAM;
				}

				// LZMA
				else
				{
					signature = CompressionSignature::LZMA;
				}

				input.seek(input.tell() - sizeof(compressMagic));
			}

			return DecompressorResult::DECOMPRESSION_SUCCES;
		}
	}
}

namespace sc
{
	namespace Decompressor {
		DecompressorResult Decompress(const fs::path& filepath, fs::path& outFilepath, MetadataCallback callback)
		{
			vector<uint8_t> buffer;

			{
				ReadFileStream file(filepath);
				buffer.resize(file.size());
				file.read(buffer.data(), file.size());
			}

			BufferStream input(&buffer);

			outFilepath = SwfCache::GetTempDirectory(filepath);

			/* Header parsing */
			CompressionSignature signature;
			std::vector<uint8_t> hash;
			DecompressorResult result = ReadHeader(input, signature, hash, callback);

			if (result != DecompressorResult::DECOMPRESSION_SUCCES)
				return result;

			bool isCached = SwfCache::IsFileCached(filepath, hash, input.size());
			if (isCached)
			{
				return DecompressorResult::DECOMPRESSION_SUCCES;
			}

			WriteFileStream output(outFilepath);

			result = CommonDecompress(input, output, signature);

			input.close();
			output.close();

			if (!isCached)
			{
				SwfCache::WriteCacheInfo(filepath, hash, input.size());
			}

			return result;
		}

		DecompressorResult Decompress(BufferStream& input, Bytestream& output, MetadataCallback callback)
		{
			CompressionSignature signature;
			std::vector<uint8_t> hash;

			DecompressorResult result = ReadHeader(input, signature, hash, callback);

			if (result != DecompressorResult::DECOMPRESSION_SUCCES)
				return result;

			return CommonDecompress(input, output, signature);
		}

		DecompressorResult Decompressor::CommonDecompress(BufferStream& input, Bytestream& output)
		{
			input.seek(0);
			uint32_t magic = input.readUInt32();
			input.read(&magic, sizeof(magic));
			input.seek(0);

			if (magic == 0x3A676953) {
				input.skip(64);
				magic = input.readUInt32();
			}

			CompressionSignature signature = getSignature(magic);

			return CommonDecompress(input, output, signature);
		}
	}
}