#pragma once

#include "DecompressException.h"

namespace sc
{
	class LzmaMissingEndMarkException : public LzmaDecompressException
	{
	public:
		virtual const char* what() const override
		{
			return "Failed to reach LZMA End Mark";
		};
	};
}