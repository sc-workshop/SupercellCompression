#pragma once

#include "GeneralException.h"

namespace sc
{
	class LzmaCompressException : public LzmaGeneralException
	{
	public:
		virtual const char* what() const override
		{
			return "Failed to compress data with LZMA compression";
		};
	};
}