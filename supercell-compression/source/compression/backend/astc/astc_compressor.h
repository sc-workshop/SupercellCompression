#pragma once

#include "compression/backend/interface/image_compressor_interface.h"
#include "compression/backend/astc/astc.h"

#include <core/preprocessor/api.h>

#include <thread>

namespace sc
{
	class WORKSHOP_API ASTCCompressor : public ImageCompressorInterface
	{
	public:
		struct Props
		{
			astc::Profile profile = astc::Profile::PRF_LDR;
			astc::Quality quality = astc::Quality::Medium;

			std::uint8_t blocks_x = 4;
			std::uint8_t blocks_y = 4;

			std::uint32_t threads_count = std::thread::hardware_concurrency();
		};

	public:
		static void write_astc_header(wk::Stream& output, std::uint16_t width, std::uint16_t height, const Props& props);

	public:
		ASTCCompressor(Props& props);
		virtual ~ASTCCompressor();

	public:
		void compress(std::uint16_t width, std::uint16_t height, wk::Image::BasePixelType pixel_type, wk::Stream& input, wk::Stream& output) override;

	private:
		astcenc_context* m_context = nullptr;
		astcenc_config* m_config = nullptr;
		std::uint32_t m_threads_count = 1;
	};
}
