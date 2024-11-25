#pragma once

#include <core/io/stream.h>

namespace sc
{
	class CompressorInterface
	{
	public:
		virtual ~CompressorInterface() = default;

	public:
		virtual void compress(wk::Stream & input, wk::Stream & output) = 0;
	};
}
