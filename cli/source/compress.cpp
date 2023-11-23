#include "main.h"
#include "SupercellCompression.h"

void LZHAM_compress(sc::Stream& input, sc::Stream& output, CommandLineOptions& options)
{
	sc::Compressor::LzhamCompressProps props;
	props.m_dict_size_log2 = 18;

	switch (options.binary.header)
	{
	case CompressionHeader::SC:
	{
		using namespace sc::ScCompression;

		Compressor::CompressorContext context;
		context.signature = Signature::Lzham;
		context.threads_count = options.threads;

		Compressor::compress(input, output, context);
	}
	break;

	case CompressionHeader::LZHAM:
	{
		output.write_int('0HZL');
		output.write_unsigned_int(props.m_dict_size_log2);
		output.write_unsigned_long(input.length());
	}
	case CompressionHeader::None:
	{
		sc::Compressor::Lzham context(props);
		context.compress_stream(input, output);
	}
	break;
	default:
		std::cout << "[ERROR] Unsupported header for LZHAM. Supported only None, SC and LZHAM" << std::endl;
		break;
	}
}

void LZMA_compress(sc::Stream& input, sc::Stream& output, CommandLineOptions& options)
{
	switch (options.binary.header)
	{
	case CompressionHeader::SC:
	{
		using namespace sc::ScCompression;

		Compressor::CompressorContext context;
		context.signature = Signature::Lzma;
		context.threads_count = options.threads;

		Compressor::compress(input, output, context);
	}
	break;

	case CompressionHeader::None:
	{
		sc::Compressor::LzmaProps props;
		props.level = 6;
		props.pb = 2;
		props.lc = 3;
		props.lp = 0;
		props.use_long_unpacked_length = options.binary.lzma.use_long_unpacked_length;
		props.numThreads = options.threads;

		sc::Compressor::Lzma context(props);
		context.compress_stream(input, output);
	}
	break;
	default:
		std::cout << "[ERROR] Unsupported header for LZMA. Supported only None and SC." << std::endl;
		break;
	}
}

void ZSTD_compress(sc::Stream& input, sc::Stream& output, CommandLineOptions& options)
{
	switch (options.binary.header)
	{
	case CompressionHeader::SC:
	{
		using namespace sc::ScCompression;

		Compressor::CompressorContext context;
		context.signature = Signature::Zstandard;
		context.threads_count = options.threads;

		Compressor::compress(input, output, context);
	}
	break;

	case CompressionHeader::None:
	{
		sc::Compressor::ZstdProps props;

		sc::Compressor::Zstd context(props);
		context.compress_stream(input, output);
	}
	break;
	default:
		std::cout << "[ERROR] Unsupported header for ZSTD. Supported only None and SC." << std::endl;
		break;
	}
}