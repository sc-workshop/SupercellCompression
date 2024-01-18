#pragma once

#include "generic/image/image.h"
#include "io/stream.h"
#include "io/memory_stream.h"

namespace sc
{
	class ImageDecompressionInterface
	{
	public:
		virtual ~ImageDecompressionInterface() = default;

	public:
		virtual void decompress_image(uint16_t widht, uint16_t height, Image::BasePixelType type, Stream& input, Stream& output) = 0;
	};
}