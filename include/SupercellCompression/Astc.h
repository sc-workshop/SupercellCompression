#pragma once

#include <stdint.h>
#include "generic/image/image.h"

#pragma region Forward Declaration

struct astcenc_config;
struct astcenc_swizzle;
struct astcenc_context;

#pragma endregion

namespace sc
{
	namespace astc
	{
#pragma region Enums
		enum class Profile : uint8_t
		{
			/** @brief The LDR sRGB color profile. */
			PRF_LDR_SRGB = 0,
			/** @brief The LDR linear color profile. */
			PRF_LDR,
			/** @brief The HDR RGB with LDR alpha color profile. */
			PRF_HDR_RGB_LDR_A,
			/** @brief The HDR RGBA color profile. */
			PRF_HDR
		};

		enum class Quality : uint8_t
		{
			Fastest = 0,
			Fast = 10,
			Medium = 60,
			Thorough = 98,
			VeryThorough = 99,
			Exhausitive = 100
		};
#pragma endregion

		astcenc_swizzle get_swizzle(Image::BasePixelType type);

		const uint8_t FileIdentifier[4] = {
			0x13, 0xAB, 0xA1, 0x5C
		};
	}
}

#include "SupercellCompression/Astc/Compressor.h"
#include "SupercellCompression/Astc/Decompressor.h"