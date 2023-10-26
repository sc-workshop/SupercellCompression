#pragma once

#include <vector>
#include <string>
#include <thread>

#include "io/stream.h"

#include "SupercellCompression/Zstd.h"
#include "SupercellCompression/Lzham.h"
#include "SupercellCompression/Lzma.h"

namespace sc {
	namespace ScCompression
	{
		enum class Signature
		{
			Lzma,
			Lzham,
			Zstandard
		};

		struct MetadataAsset
		{
			std::string name;
			std::vector<char> hash;
		};

		typedef std::vector<MetadataAsset> MetadataAssetArray;

		namespace Decompressor
		{
			void decompress(Stream& input, Stream& output, MetadataAssetArray* metadata = nullptr);
		}

		namespace Compressor
		{
			struct CompressorContext
			{
				Signature signature = Signature::Zstandard;

				bool write_assets = true;
				//MetadataAssetArray assets;

				uint32_t threads_count = std::thread::hardware_concurrency() <= 0 ? 1 : std::thread::hardware_concurrency();
			};

			void compress(Stream& input, Stream& output, CompressorContext& context);
		}
	}
}