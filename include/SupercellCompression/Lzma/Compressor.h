#pragma once
#include <thread>

#include "SupercellCompression/Lzma.h"
#include "SupercellCompression/interface/CompressionInterface.h"

namespace sc
{
	namespace Compressor
	{
		class Lzma : public CompressionInterface
		{
		public:
			struct Props
			{
				/* Compression Level: [0 - 9] */
				int level = 5;

				/* (1 << 12) <= dictSize <= (1 << 27) for 32-bit version */
				/* (1 << 12) <= dictSize <= (3 << 29) for 64-bit version */
				/*
					The maximum value for dictionary size is 1 GB = 2^30 bytes.
					Dictionary size is calculated as DictionarySize = 2^N bytes.
					For decompressing file compressed by LZMA method with dictionary
					size D = 2^N you need about D bytes of memory (RAM).
				*/
				uint32_t dict_size = (1 << 24);

				/* Number of literal context bits: [0 - 8] */
				/*
					Sometimes lc=4 gives gain for big files.
				*/
				int lc = 3;

				/* Number of literal pos bits: [0 - 4] */
				/*
					lp switch is intended for periodical data when period is
					equal 2^N. For example, for 32-bit (4 bytes)
					periodical data you can use lp=2. Often it's better to set lc0,
					if you change lp switch.
				*/
				int lp = 0;

				/* Number of pos bits: [0 - 4]*/
				/*
					pb switch is intended for periodical data
					when period is equal 2^N.
				*/
				int pb = 2;

				lzma::Mode mode = lzma::Mode::Normal;

				/* Number of fast bytes: [5 - 273]*/
				/*
					Usually big number gives a little bit better compression ratio
					and slower compression process.
				*/
				int fb = 32;

				/*Match Finder */
				lzma::BinaryMode binaryMode = lzma::BinaryMode::BinTree;

				/* 2, 3 or 4*/
				int hash_bytes_count = 4;

				/*Number of cycles for match finder: [1 - (1 << 30)]*/
				uint32_t mc = 32;

				unsigned write_end_mark = false;

				/* 1 or 2 */
				int threads = std::thread::hardware_concurrency() >= 2 ? 2 : 1;

				/* Estimated size of data that will be compressed */
				/*
					Encoder uses this value to reduce dictionary size
				*/
				uint64_t reduce_size = UINT64_MAX;

				uint64_t affinity = 0;

				/* If positive, writes the file length to a 64-bit integer, otherwise to a 32-bit integer */
				unsigned use_long_unpacked_length = true;
			};

		public:
			Lzma(Props& props);
			~Lzma();

			virtual void compress_stream(Stream& input, Stream& output);

		private:
			CLzmaEncHandle m_context;
			bool m_use_long_unpacked_data;
		};
	}
}