#pragma once

#include <astcenc.h>

#include "SupercellCompression/Astc.h"
#include "generic/image/image.h"

namespace sc
{
	namespace astc
	{
		astcenc_swizzle get_swizzle(Image::BasePixelType type)
		{
			switch (type)
			{
			case sc::Image::BasePixelType::RGB:
				return { ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_0 };

			case sc::Image::BasePixelType::LA:
				return { ASTCENC_SWZ_R, ASTCENC_SWZ_A, ASTCENC_SWZ_0, ASTCENC_SWZ_0 };

			case sc::Image::BasePixelType::L:
				return { ASTCENC_SWZ_R, ASTCENC_SWZ_0, ASTCENC_SWZ_0, ASTCENC_SWZ_0 };

			case sc::Image::BasePixelType::RGBA:
			default:
				return { ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A };
			}
		}
	}
}