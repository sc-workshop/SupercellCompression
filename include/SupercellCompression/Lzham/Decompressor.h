#pragma once
#include "SupercellCompression/DecompressionInterface.h"
#include <lzham.h>

namespace sc
{
	namespace Decompressor
	{
		struct LzhamDecompressProps : public lzham_decompress_params
		{
			size_t m_unpacked_length;

			LzhamDecompressProps()
			{
				memset(this, 0, sizeof(*this));
				m_struct_size = sizeof(lzham_decompress_params);
			}
		};

		class Lzham : public DecompressionInterface
		{
		public:
			Lzham(LzhamDecompressProps& props);
			~Lzham();

			virtual void decompress_stream(Stream& input, Stream& output);

		private:
			lzham_decompress_state_ptr m_state = nullptr;
			size_t m_unpacked_length;

			// -- Stream Buffer --
			uint8_t* m_input_buffer = nullptr;
			uint8_t* m_output_buffer = nullptr;

			static const size_t Stream_Size = 65536 * 4;
		};
	}
}