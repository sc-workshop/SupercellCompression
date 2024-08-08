#pragma once

#include <core/preprocessor/api.h>
#include <core/io/stream.h>

namespace sc
{
	namespace compression
	{
		namespace flash
		{
			struct SUPERCELL_API Decompressor
			{
				static void decompress(Stream& input, Stream& output);
			};
		}
	}
}
