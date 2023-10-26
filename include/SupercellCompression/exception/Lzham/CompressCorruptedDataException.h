#pragma once
#include "CompressException.h"

namespace sc
{
	class LzhamCompressCorruptedDataException : public LzhamCompressException
	{
		virtual const char* what() const override
		{
			return "Input buffer is corrupted. Compression is not possible";
		};
	};
}