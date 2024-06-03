#pragma once

#include "compression/backend/compressor_interface.h"

#include "compression/backend/lzham/lzham.h"

#include <thread>

namespace sc
{
	class LzhamCompressor : public CompressorInterface
	{
	public:
		struct Props
		{
			Props()
			{
				std::uint32_t threads = std::thread::hardware_concurrency();
				max_helper_threads = threads == 0 ? -1 : (threads > lzham::MAX_HELPER_THREADS ? lzham::MAX_HELPER_THREADS : threads);
			}

			std::uint32_t struct_size = sizeof(Props);
			std::uint32_t dict_size_log2 = lzham::MIN_DICT_SIZE_LOG2; // set to the log2(dictionary_size), must range between [MIN_DICT_SIZE_LOG2, MAX_DICT_SIZE_LOG2_X86] for x86 MAX_DICT_SIZE_LOG2_X64 for x64
			lzham::Level level = lzham::Level::Default;          // set to FASTEST, etc.
			std::uint32_t table_update_rate = 0;						 // Controls tradeoff between ratio and decompression throughput. 0=default, or [1,MAX_TABLE_UPDATE_RATE], higher=faster but lower ratio.
			std::int32_t max_helper_threads = -1;					 // max # of additional "helper" threads to create, must range between [-1,MAX_HELPER_THREADS], where -1=max practical
			std::uint32_t compress_flags = 0;						 // optional compression flags (see lzham_compress_flags enum)
			std::uint32_t num_seed_bytes = 0;						 // for delta compression (optional) - number of seed bytes pointed to by 'seed'
			const void* seed = nullptr;							 // for delta compression (optional) - pointer to seed bytes buffer, must be at least 'num_seed_bytes' long

			// Advanced settings - set to 0 if you don't care.
			// table_max_update_interval/m_table_update_interval_slow_rate override m_table_update_rate and allow finer control over the table update settings.
			// If either are non-zero they will override whatever m_table_update_rate is set to. Just leave them 0 unless you are specifically customizing them for your data.

			// def=0, typical range 12-128 controls the max interval between table updates, higher=longer max interval (faster decode/lower ratio). Was 16 in prev. releases.
			std::uint32_t table_max_update_interval = 64;

			// def=0, 32 or higher scaled by 32, controls the slowing of the update update freq, higher=more rapid slowing (faster decode/lower ratio). Was 40 in prev. releases.
			std::uint32_t table_update_interval_slow_rate = 64;
		};

	public:
		static void write(Stream& input, Stream& output, Props& props);

	public:
		LzhamCompressor(Props& props);
		virtual ~LzhamCompressor();

		void compress(Stream& input, Stream& output) override;

	private:
		lzham_compress_state_ptr m_state;

		// -- Stream Buffer --
		std::uint8_t* m_input_buffer = nullptr;
		std::uint8_t* m_output_buffer = nullptr;

		static const std::size_t sm_stream_size = 65536 * 4;
	};
}
