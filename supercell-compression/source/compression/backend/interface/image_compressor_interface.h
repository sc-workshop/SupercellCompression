#pragma once

#include "core/image/image.h"
#include "core/io/stream.h"

namespace sc
{
	class ImageCompressorInterface
	{
	public:
		virtual ~ImageCompressorInterface() = default;

	public:
		virtual void compress(std::uint16_t width, std::uint16_t height, wk::Image::BasePixelType pixel_type, wk::Stream& input, wk::Stream& output) = 0;
	};
}
