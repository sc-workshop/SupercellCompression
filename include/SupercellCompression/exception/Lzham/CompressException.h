#pragma once

#include "GeneralException.h"

namespace sc
{
	class LzhamCompressException : public LzhamGeneralException
	{
	public:
		virtual const char* what() const override
		{
			return "Failed to compress LZHAM data";
		};
	};
}