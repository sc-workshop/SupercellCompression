#pragma once
#include "SupercellCompression/interface/ImageCompressionInterface.h"

#include <thread>

namespace sc
{
	namespace Compressor
	{
		enum class AstcQuality : int
		{
			Fastest = 0,
			Fast = 10,
			Medium = 60,
			Thorough = 98,
			VeryThorough = 99,
			Exhausitive = 100
		};

		struct AstcCompressProps
		{
			astcenc_profile profile = ASTCENC_PRF_LDR;
			AstcQuality quality = AstcQuality::Medium;
			uint8_t blocks_x = 4;
			uint8_t blocks_y = 4;
			uint32_t threads_count = std::thread::hardware_concurrency() || 1;
		};

		class Astc : public ImageCompressionInterface
		{
		public:

			/// <summary>
			/// Compresses image and write to output with ASTC header
			/// </summary>
			/// <param name="image"></param>
			/// <param name="props"></param>
			/// <param name="output"></param>
			static void write(Image& image, AstcCompressProps props, Stream& output);

		public:
			Astc(AstcCompressProps& props);
			virtual ~Astc();

			/// <summary>
			/// Compress image data with ASTC codec
			/// </summary>
			/// <param name="image"></param>
			/// <param name="output"></param>
			virtual void compress_image(Image& image, Stream& output);

		private:
			astcenc_context* m_context;
			astcenc_config* m_config;
		};
	}
}