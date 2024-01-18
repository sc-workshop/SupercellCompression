#pragma once

#include "generic/image/image.h"
#include "io/stream.h"

namespace sc
{
	class ImageCompressionInterface
	{
	public:
		virtual ~ImageCompressionInterface() = default;

	public:
		virtual void compress_image(uint16_t widht, uint16_t height, Image::BasePixelType type, Stream& input, Stream& output) = 0;
	};
}