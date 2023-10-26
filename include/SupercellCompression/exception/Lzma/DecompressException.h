#pragma once

#include "GeneralException.h"

namespace sc
{
	class LzmaDecompressException : public LzmaGeneralException
	{
	public:
		virtual const char* what() const override
		{
			return "Failed to decompress LZMA data";
		};
	};
}