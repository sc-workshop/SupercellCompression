#pragma once

#include <string>

#include "SupercellCompression/Signature.h"
#include "SupercellBytestream.h"

#include <filesystem>
namespace fs = std::filesystem;

#include <thread>

namespace sc
{
	struct CompressorContext
	{
		uint32_t threadCount = std::thread::hardware_concurrency() <= 0 ? 1 : std::thread::hardware_concurrency();

		CompressionSignature signature = CompressionSignature::ZSTD;

		std::vector<uint8_t> metadata;
	};

	enum class CompressorResult
	{
		COMPRESSION_SUCCES,

		ALLOC_ERROR,

		LZHAM_STREAM_INIT_ERROR,
		LZHAM_CORRUPTED_DATA_ERROR,

		LZMA_STREAM_INIT_ERROR,

		ZSTD_STREAM_INIT_ERROR,
		ZSTD_CORRUPTED_DATA_ERROR
	};

	namespace Compressor
	{
		/*
		* Compress .sc file.
		*/
		CompressorResult Compress(const fs::path& inputFilepath, const fs::path& outFilepath, CompressorContext context);

		/*
		* Compress .sc file data from stream.
		*/
		CompressorResult Compress(Bytestream& inStream, Bytestream& outStream, CompressorContext context);

		/*
		* Compress common file data.
		*/
		CompressorResult CommonCompress(Bytestream& inStream, Bytestream& outStream, CompressorContext context);
	}
}
