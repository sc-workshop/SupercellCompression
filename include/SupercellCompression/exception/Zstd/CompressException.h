#pragma once

#include "GeneralException.h"

namespace sc
{
	class ZstdCompressException : public ZstdGeneralException
	{
	public:
		virtual const char* what() const override
		{
			return "Failed to compress ZSTD data";
		};
	};
}