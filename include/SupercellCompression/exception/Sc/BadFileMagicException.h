#pragma once

#include "exception/GeneralRuntimeException.h"

namespace sc
{
	class BadCompressedScFileMagic : public GeneralRuntimeException
	{
	public:
		virtual const char* what() const override
		{
			return "Compressed SC file has bad magic";
		};
	};
}