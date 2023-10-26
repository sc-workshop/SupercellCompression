#pragma once
#include "CompressException.h"

namespace sc
{
	class LzhamCompressInitException : public LzhamCompressException
	{
		virtual const char* what() const override
		{
			return "Failed to initialize LZHAM compress context";
		};
	};
}