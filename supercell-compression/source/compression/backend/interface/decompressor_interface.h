#pragma once

#include <core/io/stream.h>

namespace sc
{
	class DecompressorInterface
	{
	public:
		virtual ~DecompressorInterface() = default;

	public:
		virtual void decompress(wk::Stream& input, wk::Stream& output) = 0;
	};
}
