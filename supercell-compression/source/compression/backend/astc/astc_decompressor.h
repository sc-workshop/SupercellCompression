#pragma once

#include "compression/backend/interface/image_decompressor_interface.h"
#include "compression/backend/astc/astc.h"

#include <core/preprocessor/api.h>

#include <thread>

namespace sc
{
	class WORKSHOP_API ASTCDecompressor : public ImageDecompressorInterface
	{
	public:
		struct Props
		{
			astc::Profile profile = astc::Profile::PRF_LDR;

			std::uint8_t blocks_x = 4;
			std::uint8_t blocks_y = 4;

			std::uint32_t threads_count = std::thread::hardware_concurrency();
		};

	public:
		static void read_astc_header(wk::Stream& input, std::uint16_t& width, std::uint16_t& height, std::uint8_t& blocks_x, std::uint8_t& blocks_y);

	public:
		ASTCDecompressor(Props& props);
		virtual ~ASTCDecompressor();

		void decompress(std::uint16_t width, std::uint16_t height, wk::Image::BasePixelType pixel_type, wk::Stream& input, wk::Stream& output) override;

	private:
		astcenc_context* m_context = nullptr;
	};
}
