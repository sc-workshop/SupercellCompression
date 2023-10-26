#pragma once

#include "CompressException.h"

namespace sc
{
	class ZstdCompressInitException : public ZstdCompressException
	{
	public:
		virtual const char* what() const override
		{
			return "Failed to initialize ZSTD compression context";
		};
	};
}