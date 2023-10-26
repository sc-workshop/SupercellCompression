#pragma once
#include "DecompressException.h"

namespace sc
{
	class LzhamDecompressInitException : public LzhamDecompressException
	{
		virtual const char* what() const override
		{
			return "Failed to initialize LZHAM decompress context";
		};
	};
}