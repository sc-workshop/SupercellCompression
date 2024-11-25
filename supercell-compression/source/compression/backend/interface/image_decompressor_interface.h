#pragma once

#include "core/image/image.h"
#include "core/io/stream.h"

namespace sc
{
	class ImageDecompressorInterface
	{
	public:
		virtual ~ImageDecompressorInterface() = default;

	public:
		virtual void decompress(std::uint16_t width, std::uint16_t height, wk::Image::BasePixelType pixel_type, wk::Stream& input, wk::Stream& output) = 0;
	};
}
