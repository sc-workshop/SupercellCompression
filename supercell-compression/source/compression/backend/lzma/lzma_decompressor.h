#pragma once

#include "compression/backend/lzma/lzma.h"
#include "compression/backend/interface/decompressor_interface.h"

#include <core/preprocessor/api.h>

namespace sc
{
	// forward declare
	struct WORKSHOP_API LzmaDecompressContext;

	class LzmaDecompressor : DecompressorInterface
	{
	public:
		LzmaDecompressor(std::uint8_t header[lzma::PROPS_SIZE], const uint64_t unpacked_size);
		~LzmaDecompressor();

		void decompress(wk::Stream& input, wk::Stream& output) override;

	private:
		LzmaDecompressContext* m_context;
		std::size_t m_unpacked_size;

		std::uint8_t* m_input_buffer = nullptr;
		std::uint8_t* m_output_buffer = nullptr;

		// -- Compression Buffer --
		static const std::size_t s_stream_size = 1 << 24;
	};
}
