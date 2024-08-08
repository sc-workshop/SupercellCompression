#include "core/console/console.h"
#include "core/io/file_stream.h"
#include "core/exception/exception.h"

#include "compression/compression.h"

#include <algorithm>
#include <cctype>
#include <iostream>

namespace
{
	enum class Mode : std::uint8_t
	{
		Decompress = 0,
		Compress,
	};

	enum class Container : std::uint8_t
	{
		None = 0,
		SC,
	};

	enum class Method : std::uint8_t
	{
		LZMA = 0,
		LZHAM,
		ZSTD,
	};
}