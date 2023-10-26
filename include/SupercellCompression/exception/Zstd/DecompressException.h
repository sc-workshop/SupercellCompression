#pragma once

#include "GeneralException.h"

namespace sc
{
	class ZstdDecompressException : public ZstdGeneralException
	{
	public:
		virtual const char* what() const override
		{
			return "Failed to decompress ZSTD data";
		};
	};
}