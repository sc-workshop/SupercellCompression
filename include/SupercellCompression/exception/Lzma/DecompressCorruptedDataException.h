#pragma once

#include "DecompressException.h"

namespace sc
{
	class LzmaCorruptedDataException : public LzmaDecompressException
	{
	public:
		virtual const char* what() const override
		{
			return "LZMA data is corrupted. Decompression is not possible";
		};
	};
}