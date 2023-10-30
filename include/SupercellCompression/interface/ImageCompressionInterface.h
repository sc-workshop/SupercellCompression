#pragma once

#include "generic/image/image.h"
#include "io/stream.h"

namespace sc
{
	class ImageCompressionInterface
	{
	public:
		virtual void compress_image(Image& image, Stream& output) = 0;
	};
}