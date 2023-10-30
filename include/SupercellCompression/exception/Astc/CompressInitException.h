#pragma once
#include "CompressException.h"

namespace sc
{
	class AstcCompressInitException : public AstcCompressException
	{
		virtual const char* what() const override
		{
			return "Failed to initialize Astc compress context";
		};
	};
}