#pragma once

#include <core/io/stream.h>
#include <core/preprocessor/api.h>
#include <core/io/memory_stream.h>

namespace sc
{
	namespace compression
	{
		namespace flash
		{
			struct SUPERCELL_API Decompressor
			{
				static void decompress(Stream& input, Stream& output, sc::MemoryStream** metadata = nullptr);
			};
		}
	}
}
