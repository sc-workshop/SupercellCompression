#pragma once

#include "exception/GeneralRuntimeException.h"

namespace sc
{
	SC_CONSTRUCT_PARENT_EXCEPTION(GeneralRuntimeException, AstcGeneralException, "Failed to make ASTC operation");

#pragma region Compress
	SC_CONSTRUCT_PARENT_EXCEPTION(AstcGeneralException, AstcDecompressException, "Failed to decompress ASTC data");

	SC_CONSTRUCT_CHILD_EXCEPTION(AstcDecompressException, AstcDecompressInitException, "Failed to initialize ASTC decompress context");

#pragma endregion

#pragma region Decompress

	SC_CONSTRUCT_PARENT_EXCEPTION(AstcGeneralException, AstcCompressException, "Failed to compress ASTC data");

	SC_CONSTRUCT_CHILD_EXCEPTION(AstcCompressException, AstcCompressInitException, "Failed to initialize ASTC compress context");

#pragma endregion
}