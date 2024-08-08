#pragma once

#include "metadata.h"

#include <optional>
#include <core/io/stream.h>
#include <core/preprocessor/api.h>

namespace sc
{
	namespace compression
	{
		namespace flash
		{
			struct SUPERCELL_API Decompressor
			{
				static void decompress(Stream& input, Stream& output, std::optional<MetadataAssetArray> metadata = std::nullopt);
			};
		}
	}
}
