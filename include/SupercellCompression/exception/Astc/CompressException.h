#pragma once

#include "GeneralException.h"

namespace sc
{
	class AstcCompressException : public AstcGeneralException
	{
	public:
		virtual const char* what() const override
		{
			return "Failed to compress LZHAM data";
		};
	};
}