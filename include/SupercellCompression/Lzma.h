#pragma once

#include <stdint.h>

#include "memory/alloc.h"

#pragma region Forward Declaration
typedef void* CLzmaEncHandle;

#pragma endregion

namespace sc
{
	namespace lzma
	{
#pragma region Constants

		static const size_t PROPS_SIZE = 5;

#pragma endregion

		void* lzma_alloc(void*, size_t size);
		void lzma_free(void*, void* address);

#pragma region Enums

		enum class Mode : int
		{
			Fast = 0,
			Normal
		};

		enum class BinaryMode : int
		{
			HashChain = 0,
			BinTree
		};

#pragma endregion
	}
}

#include "Lzma/Decompressor.h"
#include "Lzma/Compressor.h"