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
			virtual void decompress(OperationContext& context, sc::Stream& input, sc::Stream& output) = 0;
			virtual void compress(OperationContext& context, sc::Stream& input, sc::Stream& output, Method method) = 0;
		};
	}
}