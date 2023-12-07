#pragma once
#include "SupercellCompression/Astc.h"
#include "SupercellCompression/interface/ImageCompressionInterface.h"

#include <thread>

namespace sc
{
	namespace Compressor
	{
		class Astc : public ImageCompressionInterface
		{
		public:
			struct Props
			{
				astc::Profile profile = astc::Profile::PRF_LDR;
				astc::Quality quality = astc::Quality::Medium;
				uint8_t blocks_x = 4;
				uint8_t blocks_y = 4;
				uint32_t threads_count = std::thread::hardware_concurrency();
			};

		public:

			/// <summary>
			/// Compresses image and write to output with ASTC header
			/// </summary>
			/// <param name="image"></param>
			/// <param name="props"></param>
			/// <param name="output"></param>
			static void write(Image& image, Props props, Stream& output);

		public:
			Astc(Props& props);
			virtual ~Astc();

			/// <summary>
			/// Compress image data with ASTC codec
			/// </summary>
			/// <param name="image"></param>
			/// <param name="output"></param>
			virtual void compress_image(uint16_t widht, uint16_t height, Image::BasePixelType type, Stream& input, Stream& output);

		private:
			astcenc_context* m_context;
			astcenc_config* m_config;
		};
	}
}