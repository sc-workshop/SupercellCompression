#pragma once

#include "exception/GeneralRuntimeException.h"

namespace sc
{
	SC_CONSTRUCT_SIMPLE_EXCEPTION(LzmaGeneralException, "Failed to make LZMA operation");

#pragma region Compress
	SC_CONSTRUCT_CHILD_EXCEPTION(LzmaGeneralException, LzmaDecompressException, "Failed to decompress LZMA data");
	SC_CONSTRUCT_CHILD_EXCEPTION(LzmaDecompressException, LzmaDecompressInitException, "Failed to initialize LZMA decompress context");
	SC_CONSTRUCT_CHILD_EXCEPTION(LzmaDecompressException, LzmaCorruptedDecompressException, "Compressed LZMA Buffer is corrupted");
	SC_CONSTRUCT_CHILD_EXCEPTION(LzmaDecompressException, LzmaMissingEndMarkException, "Failed to reach End Mark in LZMA stream");

#pragma endregion

#pragma region Decompress

	SC_CONSTRUCT_CHILD_EXCEPTION(LzmaGeneralException, LzmaCompressException, "Failed to compress LZMA data");
	SC_CONSTRUCT_CHILD_EXCEPTION(LzmaCompressException, LzmaCompressInitException, "Failed to initialize LZMA compress context");
#pragma endregion
}