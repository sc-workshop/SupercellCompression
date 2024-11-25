#pragma once

#include <optional>

#include "core/preprocessor/api.h"
#include "core/io/memory_stream.h"
#include "core/io/stream.h"

#include "flatbuffers/flexbuffers.h"

namespace sc
{
	namespace compression
	{
		namespace flash
		{
			struct WORKSHOP_API Decompressor
			{
				static std::optional<flexbuffers::Reference> decompress(wk::Stream& input, wk::Stream& output);
			};
		}
	}
}
