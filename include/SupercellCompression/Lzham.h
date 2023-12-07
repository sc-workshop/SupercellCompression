#pragma once

#include <stdint.h>

#pragma region Forward Declaration

typedef void* lzham_compress_state_ptr;
typedef void* lzham_decompress_state_ptr;

#pragma endregion

namespace sc
{
	namespace lzham
	{
#pragma region Constants
		static const uint32_t MIN_DICT_SIZE_LOG2 = 15;
		static const uint32_t MAX_DICT_SIZE_LOG2_X86 = 26;
		static const uint32_t MAX_DICT_SIZE_LOG2_X64 = 29;
		static const uint32_t MAX_HELPER_THREADS = 64;
#pragma endregion

		enum class Level : uint32_t
		{
			FASTEST = 0,
			FASTER,
			DEFAULT,
			BETTER,
			UBER,

			TOTAL_COMP_LEVELS,

			FORCE_DWORD = 0xFFFFFFFF
		};

		const uint8_t FileIdentifier[4] =
		{
			'0', 'H', 'Z', 'L'
		};
	}
}
#include "Lzham/Compressor.h"
#include "Lzham/Decompressor.h"