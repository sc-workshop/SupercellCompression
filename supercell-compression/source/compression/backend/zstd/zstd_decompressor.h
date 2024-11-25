#pragma once

#include "compression/backend/interface/decompressor_interface.h"
#include "compression/backend/zstd/zstd.h"

#include <core/preprocessor/api.h>

namespace sc
{
	class ZstdDecompressor : public DecompressorInterface
	{
	public:
		ZstdDecompressor();
		~ZstdDecompressor();

		void decompress(wk::Stream& input, wk::Stream& output) override;

	private:
		ZSTD_DStream* m_context;

		// -- Stream Buffer --
		const std::size_t Input_Buffer_Size;
		const std::size_t Output_Buffer_Size;

		std::uint8_t* m_input_buffer = nullptr;
		std::uint8_t* m_output_buffer = nullptr;
	};
}
