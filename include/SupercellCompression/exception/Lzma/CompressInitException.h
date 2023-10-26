#pragma once

#include "CompressException.h"

namespace sc
{
	class LzmaCompressInitException : public LzmaCompressException
	{
	public:
		virtual const char* what() const override
		{
			return "Failed to initialize LZMA compression context";
		};
	};
}