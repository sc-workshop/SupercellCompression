#pragma once

#include "compression/backend/sc/signature.h"

#include <core/preprocessor/api.h>
#include <core/io/stream.h>

#include <thread>

namespace sc
{
	namespace compression
	{
		namespace flash
		{
			struct SUPERCELL_API Compressor
			{
				struct Context
				{
					Signature signature = Signature::Zstandard;

					bool contains_metadata = false;

					uint32_t threads_count = std::thread::hardware_concurrency() <= 0 ? 1 : std::thread::hardware_concurrency();
				};

				static void compress(Stream& input, Stream& output, Context& context);
			};
		}
	}
}
