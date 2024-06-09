#pragma once

#include "core/image/image.h"

// forward declare
struct astcenc_context;
struct astcenc_config;
struct astcenc_swizzle;

namespace sc
{
	namespace astc
	{
		const std::uint8_t file_identifier[4] = {
			0x13, 0xAB, 0xA1, 0x5C
		};

		enum class Profile : std::uint8_t
		{
			PRF_LDR_SRGB = 0,
			PRF_LDR,

			PRF_HDR_RGB_LDR_A,
			PRF_HDR
		};

		enum class Quality : std::uint8_t
		{
			Fastest = 0,
			Fast = 10,
			Medium = 60,
			Thorough = 98,
			VeryThorough = 99,
			Exhausitive = 100
		};

		astcenc_swizzle get_swizzle(Image::BasePixelType type);
	}
}
