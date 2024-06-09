#pragma once

#include <core/io/stream.h>

namespace sc
{
	class CompressorInterface
	{
	public:
		virtual ~CompressorInterface() = default;

	public:
		virtual void compress(Stream & input, Stream & output) = 0;
	};
}
