#pragma once

#include "exception/GeneralRuntimeException.h"

namespace sc
{
	SC_CONSTRUCT_PARENT_EXCEPTION(GeneralRuntimeException, LzhamGeneralException, "Failed to make LZHAM operation");

#pragma region Compress
	SC_CONSTRUCT_PARENT_EXCEPTION(LzhamGeneralException, LzhamDecompressException, "Failed to decompress LZHAM data");

	SC_CONSTRUCT_CHILD_EXCEPTION(LzhamDecompressException, LzhamDecompressInitException, "Failed to initialize LZHAM decompress context");
	SC_CONSTRUCT_CHILD_EXCEPTION(LzhamDecompressException, LzhamCorruptedDecompressException, "Compressed LZHAM Buffer is corrupted");

#pragma endregion

#pragma region Decompress

	SC_CONSTRUCT_PARENT_EXCEPTION(LzhamGeneralException, LzhamCompressException, "Failed to compress LZHAM data");

	SC_CONSTRUCT_CHILD_EXCEPTION(LzhamCompressException, LzhamCompressInitException, "Failed to initialize LZHAM compress context");
#pragma endregion
}