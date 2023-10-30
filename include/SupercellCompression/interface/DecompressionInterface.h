#pragma once
#include "io/stream.h"

namespace sc
{
	namespace Decompressor
	{
		class DecompressionInterface
		{
		public:
			virtual void decompress_stream(Stream& input, Stream& output) = 0;
		};
	}
}