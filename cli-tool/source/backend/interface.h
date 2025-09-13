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
			virtual ~CommandLineInterface() = default;

		public:
			virtual void parse(wk::ArgumentParser& parser) = 0;
			virtual void decompress(OperationContext& context, wk::Stream& input, wk::Stream& output) = 0;
			virtual void compress(OperationContext& context, wk::Stream& input, wk::Stream& output, Method method) = 0;
		};
	}
}