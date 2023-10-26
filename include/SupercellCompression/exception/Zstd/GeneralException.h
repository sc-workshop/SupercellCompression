#pragma once

#include "exception/GeneralRuntimeException.h"

namespace sc
{
	class ZstdGeneralException : public GeneralRuntimeException
	{
	public:
		virtual const char* what() const override
		{
			return "Failed to make ZSTD operation";
		};
	};
}