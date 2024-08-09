#pragma once

#include "signature.h"

#include <thread>
#include <core/memory/ref.h>
#include <core/io/stream.h>
#include <core/preprocessor/api.h>

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

					Ref<Stream> metadata = nullptr;

					uint32_t threads_count = std::thread::hardware_concurrency() <= 0 ? 1 : std::thread::hardware_concurrency();
				};

				static void compress(Stream& input, Stream& output, Context& context);
			};
		}
	}
}
