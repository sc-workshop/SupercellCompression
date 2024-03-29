#include "SupercellCompression/ScCompression.h"
#include "generic/md5.h"
#include "io/buffer_stream.h"

#define HASH_LENGTH 16

namespace sc {
	namespace ScCompression
	{
		namespace Compressor
		{
			const char* metadata_delim = "START";

			void write_metadata(Stream& input)
			{
				// TODO !!!!!!!!!!!!!!!!!!!!!!
				input.write(metadata_delim, sizeof(metadata_delim));

				sc::BufferStream metadata;

				// metadata_flags
				metadata.write_byte(0);

				// data_info_offset_field_size
				metadata.write_byte(0);

				input.write(metadata.data(), metadata.length());
				input.write_unsigned_int(static_cast<uint32_t>(metadata.length()), Endian::Big);
			}

			void compress(Stream& input, Stream& output, CompressorContext& context)
			{
				using namespace sc::Compressor;

				output.write_unsigned_short(SC_MAGIC);

				if (context.write_assets)
				{
					output.write_int(4, Endian::Big);
				}

				// Signature check
				switch (context.signature)
				{
				case Signature::Lzma:
				case Signature::Lzham:
					output.write_int(1, Endian::Big);
					break;
				case Signature::Zstandard:
					output.write_int(3, Endian::Big);
					break;
				}

				// Hash
				{
					md5 md_ctx;
					uint8_t hash[HASH_LENGTH];

					md_ctx.update((uint8_t*)input.data(), input.length());

					md_ctx.final(hash);

					output.write_unsigned_int(HASH_LENGTH, Endian::Big);
					output.write(&hash, HASH_LENGTH);
				}

				switch (context.signature)
				{
				case Signature::Lzma:
				{
					Lzma::Props props;
					props.level = 6;
					props.pb = 2;
					props.lc = 3;
					props.lp = 0;
					props.threads = context.threads_count > 1 ? 2 : 1;
					props.dict_size = 262144;
					props.use_long_unpacked_length = false;

					if (input.length() > 1 << 28)
						props.lc = 4;

					sc::Compressor::Lzma compression(props);
					compression.compress_stream(input, output);
				}
				break;

				case Signature::Lzham:
				{
					const uint32_t dictionary_size = 18;

					output.write_unsigned_int(SCLZ_MAGIC);
					output.write_unsigned_byte(dictionary_size);
					output.write_unsigned_int(static_cast<uint32_t>(input.length() - input.position()));

					Lzham::Props props;
					props.dict_size_log2 = dictionary_size;
					props.max_helper_threads = static_cast<uint16_t>(context.threads_count > 0 && context.threads_count < lzham::MAX_HELPER_THREADS ? context.threads_count : -1);

					sc::Compressor::Lzham compression(props);
					compression.compress_stream(input, output);
				}
				break;

				case Signature::Zstandard:
				{
					Zstd::Props props;
					props.compression_level = 16;
					props.checksum_flag = false;
					props.content_size_flag = true;
					props.workers_count = context.threads_count;

					sc::Compressor::Zstd compression(props);
					compression.compress_stream(input, output);
				}
				break;
				}

				if (context.write_assets)
				{
					write_metadata(output);
				}
			}
		}
	}
}