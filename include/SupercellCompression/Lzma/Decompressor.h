#pragma once
#include "SupercellCompression/interface/DecompressionInterface.h"

namespace sc
{
	namespace Decompressor
	{
		class Lzma : public DecompressionInterface
		{
		public:
			Lzma(Byte header[LZMA_PROPS_SIZE], const UInt64 unpacked_size);
			~Lzma();

			virtual void decompress_stream(Stream& input, Stream& output) override;

		private:
			CLzmaDec m_context;
			size_t m_unpacked_size;

			// -- Compression Buffer --
			static const size_t Stream_Size = 1 << 24;

			uint8_t* m_input_buffer = nullptr;
			uint8_t* m_output_buffer = nullptr;
		};
	}
}