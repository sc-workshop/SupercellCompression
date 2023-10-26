#pragma once
#include "SupercellCompression/Lzham.h"
#include "SupercellCompression/CompressionInterface.h"

namespace sc
{
	namespace Compressor
	{
		struct LzhamCompressProps : public lzham_compress_params
		{
			LzhamCompressProps()
			{
				memset(this, 0, sizeof(*this));
				m_struct_size = sizeof(lzham_compress_params);
				m_max_helper_threads = -1;
			}
		};

		class Lzham : public CompressionInterface
		{
		public:
			Lzham(LzhamCompressProps& props);
			~Lzham();

			virtual void compress_stream(Stream& input, Stream& output);

		private:
			lzham_compress_state_ptr m_state;

			// -- Stream Buffer --
			uint8_t* m_input_buffer = nullptr;
			uint8_t* m_output_buffer = nullptr;

			static const size_t Stream_Size = 65536 * 4;
		};
	}
}