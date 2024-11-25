#pragma once

#include "compression/backend/sc/signature.h"

#include "core/preprocessor/api.h"
#include "core/io/stream.h"
#include "core/memory/ref.h"

#include <thread>
#include <optional>

#include "flatbuffers/flexbuffers.h"

namespace sc
{
	namespace compression
	{
		namespace flash
		{
			struct WORKSHOP_API Compressor
			{
				struct Context
				{
					Signature signature = Signature::Zstandard;
					std::optional<flexbuffers::Builder> metadata;
					uint32_t threads_count = std::thread::hardware_concurrency() <= 0 ? 1 : std::thread::hardware_concurrency();
				};

				static void compress(wk::Stream& input, wk::Stream& output, Context& context);
			};
		}
	}
}
