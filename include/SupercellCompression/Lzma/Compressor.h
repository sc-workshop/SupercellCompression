#pragma once
#include "SupercellCompression/interface/CompressionInterface.h"

namespace sc
{
	namespace Compressor
	{
		struct LzmaProps : public CLzmaEncProps
		{
			LzmaProps()
			{
				LzmaEncProps_Init(this);
			};

			// Writes length of uncompressed data in 8 bytes
			bool use_long_unpacked_length = false;
		};

		class Lzma : public CompressionInterface
		{
		public:
			Lzma(LzmaProps& props);
			~Lzma();

			virtual void compress_stream(Stream& input, Stream& output);

		private:
			CLzmaEncHandle m_context;
			bool m_use_long_unpacked_data;
		};
	}
}