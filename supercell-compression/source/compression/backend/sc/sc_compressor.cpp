#include "compression/backend/sc/sc_compressor.h"

#include "compression/backend/lzma/lzma_compressor.h"
#include "compression/backend/lzham/lzham_compressor.h"
#include "compression/backend/zstd/zstd_compressor.h"

#include <core/io/buffer_stream.h>
#include <core/crypto/md5.h>

using namespace wk;

namespace sc
{
	namespace compression
	{
		namespace flash
		{
			void Compressor::compress(Stream& input, Stream& output, Context& context)
			{
				output.write_unsigned_short(SC_MAGIC);

				bool write_metadata = context.metadata.has_value();
				if (write_metadata)
				{
					output.write_int(4, Endian::Big);
				}

				// signature check
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

				// hash MD5
				{
					MD5::md5 md_ctx;
					std::uint8_t hash[MD5::HASH_LENGTH];

					md_ctx.update((std::uint8_t*)input.data(), input.length());

					md_ctx.final(hash);

					output.write_unsigned_int((uint32_t)MD5::HASH_LENGTH, Endian::Big);
					output.write(&hash, MD5::HASH_LENGTH);
				}

				// Compressing file content
				compress(input, output, context.signature);

				if (write_metadata)
				{
					context.metadata->Finish();
					size_t metadata_length = context.metadata->GetSize();

					output.write(flash::SC_START, 5);
					output.write(context.metadata->GetBuffer().data(), metadata_length);
					output.write_unsigned_int(static_cast<uint32_t>(metadata_length), Endian::Big);
				}
			}

			void Compressor::compress(wk::Stream& input, wk::Stream& output, Signature signature)
			{
				switch (signature)
				{
				case Signature::Lzma:
				{
					LzmaCompressor::Props props;

					props.level = 6;
					props.pb = 2;
					props.lc = 3;
					props.lp = 0;
					props.threads = 2;
					props.dict_size = 262144;
					props.use_long_unpacked_length = false;

					if (input.length() > 1 << 28)
						props.lc = 4;

					LzmaCompressor compression(props);
					compression.compress(input, output);
				}
				break;

				case Signature::Lzham:
				{
					const std::uint32_t dictionary_size = 18;

					output.write_unsigned_int(SC_LZHAM_MAGIC);
					output.write_unsigned_byte(dictionary_size);
					output.write_unsigned_int(static_cast<std::uint32_t>(input.length() - input.position()));

					LzhamCompressor::Props props;

					props.dict_size_log2 = dictionary_size;
					props.max_helper_threads = lzham::MAX_HELPER_THREADS;

					LzhamCompressor compression(props);
					compression.compress(input, output);
				}
				break;

				case Signature::Zstandard:
				{
					ZstdCompressor::Props props;

					props.compression_level = 16;
					props.checksum_flag = false;
					props.content_size_flag = true;

					ZstdCompressor compression(props);
					compression.compress(input, output);
				}
				break;
				}
			}
		}
	}
}