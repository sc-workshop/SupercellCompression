#pragma once
#include "DecompressException.h"

namespace sc
{
	class AstcDecompressInitException : public AstcDecompressException
	{
		virtual const char* what() const override
		{
			return "Failed to initialize ASTC decompress context";
		};
	};
}