#pragma once

#include "exception/GeneralRuntimeException.h"

namespace sc
{
	SC_CONSTRUCT_PARENT_EXCEPTION(GeneralRuntimeException, ZstdGeneralException, "Failed to make ZSTD operation");

#pragma region Compress
	SC_CONSTRUCT_PARENT_EXCEPTION(ZstdGeneralException, ZstdDecompressException, "Failed to decompress ZSTD data");

	SC_CONSTRUCT_CHILD_EXCEPTION(ZstdDecompressException, ZstdDecompressInitException, "Failed to initialize ZSTD decompress context");
	SC_CONSTRUCT_CHILD_EXCEPTION(ZstdDecompressException, ZstdCorruptedDecompressException, "Compressed ZSTD Buffer is corrupted");

#pragma endregion

#pragma region Decompress

	SC_CONSTRUCT_PARENT_EXCEPTION(ZstdGeneralException, ZstdCompressException, "Failed to compress ZSTD data");

	SC_CONSTRUCT_CHILD_EXCEPTION(ZstdCompressException, ZstdCompressInitException, "Failed to initialize ZSTD compress context");

#pragma endregion
}