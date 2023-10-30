#pragma once

#include "exception/GeneralRuntimeException.h"

namespace sc
{
	class ImageAlreadyCompressedException : public GeneralRuntimeException
	{
	public:
		virtual const char* what() const override
		{
			return "Image is already compressed. Another compression is not possible.";
		};
	};
}