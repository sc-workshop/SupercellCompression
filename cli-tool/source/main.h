#include <algorithm>
#include <cctype>
#include <iostream>

#include "core/console/console.h"
#include "core/io/file_stream.h"
#include "core/exception/exception.h"

#include "compression.h"

namespace
{
	enum class Mode : uint8_t
	{
		Decompress = 0,
		Compress,
	};

	enum class Container : uint8_t
	{
		None = 0,
		SC,
	};

	enum class Method : uint8_t
	{
		LZMA = 0,
		LZHAM,
		ZSTD,
	};
}

namespace
{
	struct OperationContext
	{
		int current_index = -1;
		fs::path current_file;
		fs::path current_output_file;
	};
}