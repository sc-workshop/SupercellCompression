#pragma once
#include <astcenc.h>
#include "generic/image/image.h"

static const uint8_t AstcFileIdentifier[4] = {
	0x13, 0xAB, 0xA1, 0x5C
};

namespace sc
{
	astcenc_swizzle get_astc_swizzle(Image::BasePixelType type);
}

#include "SupercellCompression/Astc/Compressor.h"
#include "SupercellCompression/Astc/Decompressor.h"