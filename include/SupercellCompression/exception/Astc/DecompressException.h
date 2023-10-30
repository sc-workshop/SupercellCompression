#pragma once

#include "GeneralException.h"

namespace sc
{
	class AstcDecompressException : public AstcGeneralException
	{
	public:
		virtual const char* what() const override
		{
			return "Failed to decompress ASTC data";
		};
	};
}