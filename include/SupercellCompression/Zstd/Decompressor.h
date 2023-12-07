#pragma once
#include "SupercellCompression/Zstd.h"
#include "SupercellCompression/interface/DecompressionInterface.h"

namespace sc
{
	namespace Decompressor
	{
		class Zstd : public DecompressionInterface
		{
		public:
			Zstd();
			~Zstd();

			virtual void decompress_stream(Stream& input, Stream& output);

		private:
			ZSTD_DStream* m_context;

			// -- Stream Buffer --
			const size_t Input_Buffer_Size;
			const size_t Output_Buffer_Size;

			uint8_t* m_input_buffer = nullptr;
			uint8_t* m_output_buffer = nullptr;
		};
	}
}