#pragma once
#include "SupercellCompression/interface/ImageDecompressionInterface.h"
#include <astcenc_internal.h>

namespace sc
{
	namespace Decompressor
	{
		struct AstcDecompressProps
		{
			astcenc_profile profile = ASTCENC_PRF_LDR;

			uint8_t blocks_x;
			uint8_t blocks_y;

			uint32_t threads_count = std::thread::hardware_concurrency();
		};

		class Astc : public ImageDecompressionInterface
		{
		public:
			Astc(AstcDecompressProps& props);

			~Astc();

		public:
			virtual void decompress_image(uint16_t width, uint16_t height, Image::BasePixelType type, Stream& input, Stream& output);

		private:
			astcenc_context* m_context;
		};
	}
}
