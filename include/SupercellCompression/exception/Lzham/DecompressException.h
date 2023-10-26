#pragma once

#include "GeneralException.h"

namespace sc
{
	class LzhamDecompressException : public LzhamGeneralException
	{
	public:
		virtual const char* what() const override
		{
			return "Failed to decompress LZHAM data";
		};
	};
}