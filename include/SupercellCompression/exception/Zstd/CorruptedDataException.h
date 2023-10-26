#pragma once

#include "DecompressException.h"

namespace sc
{
	class ZstdCorruptedDataException : public ZstdDecompressException
	{
	public:
		virtual const char* what() const override
		{
			return "ZSTD data is corrupted. Decompression is not possible";
		};
	};
}