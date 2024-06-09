#include "compression/backend/astc/astc.h"

#include <astcenc.h>

namespace sc
{
	namespace astc
	{
		astcenc_swizzle get_swizzle(Image::BasePixelType type)
		{
			switch (type)
			{
			case Image::BasePixelType::RGB:
				return { ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_0 };

			case Image::BasePixelType::LA:
				return { ASTCENC_SWZ_R, ASTCENC_SWZ_A, ASTCENC_SWZ_0, ASTCENC_SWZ_0 };

			case Image::BasePixelType::L:
				return { ASTCENC_SWZ_R, ASTCENC_SWZ_0, ASTCENC_SWZ_0, ASTCENC_SWZ_0 };

			case Image::BasePixelType::RGBA:
			default:
				return { ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A };
			}
		}
	}
}
