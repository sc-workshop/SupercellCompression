#pragma once

#include <SupercellBytestream/base/BytestreamBase.h>
#include "SupercellCompression/Compressor.h"
#include "SupercellCompression/Decompressor.h"

namespace sc
{
	namespace LZHAM
	{
		CompressorResult Compress(BytestreamBase& input, BytestreamBase& output, uint32_t theards);
		DecompressorResult Decompress(BytestreamBase& input, BytestreamBase& output);
	}
}