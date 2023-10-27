#pragma once

#include "memory/alloc.h"
#include <stdint.h>
#include <Alloc.h>
#include <LzmaDec.h>
#include <LzmaEnc.h>

namespace sc
{
	static void* LzAlloc(ISzAllocPtr, size_t size)
	{
		return sc::memalloc(size);
	}
	static void LzFree(ISzAllocPtr, void* address) {
		free(address);
	}
	const ISzAlloc LzAllocObj = { LzAlloc, LzFree };
}

#include "Lzma/Decompressor.h"
#include "Lzma/Compressor.h"