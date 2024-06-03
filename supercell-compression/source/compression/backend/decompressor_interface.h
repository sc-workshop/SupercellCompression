#pragma once

#include <core/io/stream.h>

namespace sc
{
	class DecompressorInterface
	{
	public:
		virtual ~DecompressorInterface() = default;

	public:
		virtual void decompress(Stream& input, Stream& output) = 0;
	};
}
