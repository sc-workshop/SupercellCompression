#include "main.h"
#include "SupercellCompression.h"

using namespace sc::Compressor;

void LZHAM_compress(sc::Stream& input, sc::Stream& output, CommandLineOptions& options)
{
	Lzham::Props props;
	props.dict_size_log2 = 18;

	switch (options.binary.container)
	{
	case FileContainer::SC:
	{
		using namespace sc::ScCompression;

		Compressor::CompressorContext context;
		context.signature = Signature::Lzham;
		context.threads_count = options.threads;

		Compressor::compress(input, output, context);
	}
	break;

	case FileContainer::None:
		Lzham::write(input, output, props);
		break;

	default:
		std::cout << "[ERROR] Unsupported container for LZHAM. Supported only None, SC and LZHAM" << std::endl;
		break;
	}
}

void LZMA_compress(sc::Stream& input, sc::Stream& output, CommandLineOptions& options)
{
	switch (options.binary.container)
	{
	case FileContainer::SC:
	{
		using namespace sc::ScCompression;

		Compressor::CompressorContext context;
		context.signature = Signature::Lzma;
		context.threads_count = options.threads;

		Compressor::compress(input, output, context);
	}
	break;

	case FileContainer::None:
	{
		Lzma::Props props;
		props.level = 6;
		props.pb = 2;
		props.lc = 3;
		props.lp = 0;
		props.use_long_unpacked_length = options.binary.lzma.use_long_unpacked_length;
		props.threads = options.threads;

		Lzma context(props);
		context.compress_stream(input, output);
	}
	break;
	default:
		std::cout << "[ERROR] Unsupported container for LZMA. Supported only None and SC." << std::endl;
		break;
	}
}

void ZSTD_compress(sc::Stream& input, sc::Stream& output, CommandLineOptions& options)
{
	switch (options.binary.container)
	{
	case FileContainer::SC:
	{
		using namespace sc::ScCompression;

		Compressor::CompressorContext context;
		context.signature = Signature::Zstandard;
		context.threads_count = options.threads;

		Compressor::compress(input, output, context);
	}
	break;

	case FileContainer::None:
	{
		Zstd::Props props;
		// TODO: more params

		sc::Compressor::Zstd context(props);
		context.compress_stream(input, output);
	}
	break;
	default:
		std::cout << "[ERROR] Unsupported container for ZSTD. Supported only None and SC." << std::endl;
		break;
	};
}

bool binary_compressing(sc::Stream& input_stream, sc::Stream& output_stream, CommandLineOptions& options)
{
	switch (options.binary.method)
	{
	case CompressionMethod::LZMA:
		LZMA_compress(input_stream, output_stream, options);
		break;

	case CompressionMethod::ZSTD:
		ZSTD_compress(input_stream, output_stream, options);
		break;

	case CompressionMethod::LZHAM:
		LZHAM_compress(input_stream, output_stream, options);
		break;

	default:
		std::cout << "[ERROR] Unknown compression method" << std::endl;
		return false;
	}

	return true;
}