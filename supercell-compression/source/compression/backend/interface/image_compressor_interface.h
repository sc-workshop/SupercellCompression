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
		virtual void compress(std::uint16_t width, std::uint16_t height, Image::BasePixelType pixel_type, Stream& input, Stream& output) = 0;
	};
}
