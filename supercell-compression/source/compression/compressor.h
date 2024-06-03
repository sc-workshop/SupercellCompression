#pragma once

#include <core/io/stream.h>

#include "compression/signature.h"

#include <thread>

namespace sc
{
	struct Compressor
	{
		struct Context
		{
			Signature signature = Signature::Zstandard;

			bool write_assets = false;
			// TODO(pavidloq): metadata generation

			uint32_t threads_count = std::thread::hardware_concurrency() <= 0 ? 1 : std::thread::hardware_concurrency();
		};

		static void compress(Stream& input, Stream& output, Context& context);
	};
}
