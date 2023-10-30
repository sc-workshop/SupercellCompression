#pragma once

#include "exception/GeneralRuntimeException.h"

namespace sc
{
	class AstcGeneralException : public GeneralRuntimeException
	{
	public:
		virtual const char* what() const override
		{
			return "Failed to make ASTC operation";
		};
	};
}