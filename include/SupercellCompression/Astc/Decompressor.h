#pragma once

#include "SupercellCompression/interface/ImageDecompressionInterface.h"
#include "generic/image/raw_image.h"

namespace sc
{
	namespace Decompressor
	{
		class Astc : public ImageDecompressionInterface
		{
		public:
			struct Props
			{
				astc::Profile profile = astc::Profile::PRF_LDR;

				uint8_t blocks_x = 4;
				uint8_t blocks_y = 4;

				uint32_t threads_count = std::thread::hardware_concurrency();
			};
		public:
			/// <summary>
			/// Reads ASTC file header
			/// </summary>
			static void read_header(Stream& buffer, uint16_t& width, uint16_t& height, uint8_t& blocks_x, uint8_t& blocks_y);

		public:
			Astc(Props& props);

			~Astc();

		public:
			virtual void decompress_image(uint16_t width, uint16_t height, Image::BasePixelType type, Stream& input, Stream& output);

		private:
			astcenc_context* m_context;
		};
	}
}
