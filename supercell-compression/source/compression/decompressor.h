#pragma once

#include <core/io/stream.h>

#include "compression/metadata.h"

namespace sc
{
	struct Decompressor
	{
		static void decompress(Stream& input, Stream& output, MetadataAssetArray* metadata = nullptr);
	};
}
