#pragma once

#include <core/console/console.h>
#include <core/io/stream.h>

namespace sc
{
	namespace compression
	{
		class CommandLineInterface
		{
		public:
			virtual void parse(sc::ArgumentParser& parser) = 0;
			virtual void decompress(sc::Stream& input, sc::Stream& output) = 0;
			virtual void compress(sc::Stream& input, sc::Stream& output, Method method) = 0;
		};
	}
}