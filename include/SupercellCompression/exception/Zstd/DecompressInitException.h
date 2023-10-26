#pragma once

#include "DecompressException.h"

namespace sc
{
	class ZstdDecompressInitException : public ZstdDecompressException
	{
	public:
		virtual const char* what() const override
		{
			return "Failed to initialize ZSTD decompress context";
		};
	};
}