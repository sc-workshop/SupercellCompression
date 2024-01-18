#pragma once
#include "SupercellCompression/Lzham.h"
#include "SupercellCompression/interface/DecompressionInterface.h"
//#include <lzham.h>

namespace sc
{
	namespace Decompressor
	{
		class Lzham : public DecompressionInterface
		{
		public:
			struct Props
			{
				uint32_t struct_size = sizeof(Props);
				uint32_t dict_size_log2 = lzham::MIN_DICT_SIZE_LOG2;         // set to the log2(dictionary_size), must range between [MIN_DICT_SIZE_LOG2, MAX_DICT_SIZE_LOG2_X86] for x86 LZHAM_MAX_DICT_SIZE_LOG2_X64 for x64
				uint32_t table_update_rate = 0;								 // Controls tradeoff between ratio and decompression throughput. 0=default, or [1,LZHAM_MAX_TABLE_UPDATE_RATE], higher=faster but lower ratio.
				uint32_t decompress_flags = 0;								 // optional decompression flags (see lzham_decompress_flags enum)
				uint32_t num_seed_bytes = 0;								 // for delta compression (optional) - number of seed bytes pointed to by m_pSeed_bytes
				const void* seed = nullptr;									 // for delta compression (optional) - pointer to seed bytes buffer, must be at least m_num_seed_bytes long

				// Advanced settings - set to 0 if you don't care.
				  // 'table_max_update_interval'/'table_update_interval_slow_rate override' 'table_update_rate' and allow finer control over the table update settings.
				  // If either are non-zero they will override whatever m_table_update_rate is set to. Just leave them 0 unless you are specifically customizing them for your data.

				// def=0, typical range 12-128, controls the max interval between table updates, higher=longer max interval (faster decode/lower ratio). Was 16 in prev. releases.
				uint32_t table_max_update_interval = 64;
				// def=0, 32 or higher, scaled by 32, controls the slowing of the update update freq, higher=more rapid slowing (faster decode/lower ratio). Was 40 in prev. releases.
				uint32_t table_update_interval_slow_rate = 64;

				size_t unpacked_length;
			};
		public:
			Lzham(Props& props);
			virtual ~Lzham();

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