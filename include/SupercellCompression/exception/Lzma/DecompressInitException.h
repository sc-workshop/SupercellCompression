#pragma once

#include "DecompressException.h"

namespace sc
{
	class LzmaDecompressInitException : public LzmaDecompressException
	{
	public:
		virtual const char* what() const override
		{
			return "Failed to initialize LZMA decompression context";
		};
	};
}