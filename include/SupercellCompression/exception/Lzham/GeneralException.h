#pragma once

#include "exception/GeneralRuntimeException.h"

namespace sc
{
	class LzhamGeneralException : public GeneralRuntimeException
	{
	public:
		virtual const char* what() const override
		{
			return "Failed to make LZHAM operation";
		};
	};
}