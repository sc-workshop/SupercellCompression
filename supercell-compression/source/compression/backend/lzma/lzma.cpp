#include "compression/backend/lzma/lzma.h"

#include <core/io/stream.h>

#include <Alloc.h>
#include <LzmaEnc.h>

using namespace wk;

namespace sc
{
	namespace lzma
	{
		void* lzma_alloc(void*, std::size_t size)
		{
			return Memory::allocate(size);
		}

		void lzma_free(void*, void* address)
		{
			Memory::free(address);
		}
	}

	const void* LzmaAlloc[2] = { reinterpret_cast<void*>(&lzma::lzma_alloc), reinterpret_cast<void*>(&lzma::lzma_free)};
}
