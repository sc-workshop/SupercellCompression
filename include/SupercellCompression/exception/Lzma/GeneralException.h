#pragma once

#include "exception/GeneralRuntimeException.h"

namespace sc
{
	class LzmaGeneralException : public GeneralRuntimeException
	{
	public:
		virtual const char* what() const override
		{
			return "Failed to make LZMA operation";
		};
	};
}