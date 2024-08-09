#pragma once


#include <core/preprocessor/api.h>
#include <core/io/memory_stream.h>
#include <core/io/stream.h>
#include <core/preprocessor/api.h>

namespace sc
{
	namespace compression
	{
		namespace flash
		{
				static void decompress(Stream& input, Stream& output, sc::MemoryStream** metadata = nullptr);
			{
				static void decompress(Stream& input, Stream& output, std::optional<MetadataAssetArray> metadata = std::nullopt);
			};
		}
	}
}
