#pragma once

#include <core/memory/memory.h>

// forward declare
typedef void* CLzmaEncHandle;

namespace sc
{
	namespace lzma
	{
		static const std::size_t PROPS_SIZE = 5;

		void* lzma_alloc(void*, std::size_t size);
		void lzma_free(void*, void* address);

		enum class Mode : std::int32_t
		{
			Fast = 0,
			Normal
		};

		enum class BinaryMode : std::int32_t
		{
			HashChain = 0,
			BinTree
		};
	}

	extern const void* LzmaAlloc[2];
}
