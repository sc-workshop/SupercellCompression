#pragma once

#include <SupercellBytestream/base/BytestreamBase.h>
#include "SupercellCompression/Compressor.h"
#include "SupercellCompression/Decompressor.h"

namespace sc
{
	namespace LZMA
	{
		DecompressorResult Decompress(BytestreamBase& input, BytestreamBase& output);
		CompressorResult Compress(BytestreamBase& input, BytestreamBase& output, uint32_t threads);
	}
}