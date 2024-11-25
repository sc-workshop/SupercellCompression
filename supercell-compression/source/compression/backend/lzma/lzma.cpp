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

	const void* LzmaAlloc[] = { (void*)&lzma::lzma_alloc, (void*)&lzma::lzma_free };
}
