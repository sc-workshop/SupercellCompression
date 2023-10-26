#pragma once

#include "CompressException.h"

namespace sc
{
	class LzmaCompressPropsException : public LzmaCompressException
	{
	public:
		virtual const char* what() const override
		{
			return "Some of LZMA compression props are invalid";
		};
	};
}