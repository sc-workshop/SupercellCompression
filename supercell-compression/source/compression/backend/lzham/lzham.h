#pragma once

#include <cstdint>

// forward declare
typedef void* lzham_compress_state_ptr;
typedef void* lzham_decompress_state_ptr;

namespace sc
{
	namespace lzham
	{
		static const std::uint32_t MIN_DICT_SIZE_LOG2 = 15;
		static const std::uint32_t MAX_DICT_SIZE_LOG2_X86 = 26;
		static const std::uint32_t MAX_DICT_SIZE_LOG2_X64 = 29;
		static const std::uint32_t MAX_HELPER_THREADS = 64;

		enum class Level : std::uint32_t
		{
			Fastest = 0,
			Faster,
			Default,
			Better,
			Uber,

			TotalCompLevels,

			ForceDWORD = 0xFFFFFFFF
		};

		const std::uint8_t file_identifier[4] =
		{
			'0', 'H', 'Z', 'L'
		};
	}
}
