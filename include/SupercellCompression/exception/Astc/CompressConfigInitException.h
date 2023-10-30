#pragma once
#include "CompressException.h"

namespace sc
{
	class AstcCompressConfigInitException : public AstcCompressException
	{
		virtual const char* what() const override
		{
			return "Failed to initialize ASTC compress config";
		};
	};
}