#pragma once

#include "SupercellBytestream.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace sc
{
	enum class DecompressorResult
	{
		DECOMPRESSION_SUCCES,

		ALLOC_ERROR,
		CORRUPTED_HEADER_ERROR,
		STREAM_ERROR,

		LZHAM_CORRUPTED_DICT_SIZE_ERROR,
		LZHAM_STREAM_INIT_ERROR,
		LZHAM_CORRUPTED_DATA_ERROR,

		LZMA_CORRUPTED_DATA_ERROR,
		LZMA_MISSING_END_MARKER_ERROR,

		ZSTD_CORRUPTED_DATA_ERROR,
		ZSTD_STREAM_INIT_ERROR
	};

	namespace Decompressor
	{
		/**
		 * Decompress file and then store it in cache, without need to decompress in the future.
		 */
		DecompressorResult Decompress(const fs::path& filepath, fs::path& outFilepath);

		/**
		 * Decompress file from stream.
		 */
		DecompressorResult Decompress(Bytestream& input, Bytestream& output);

		/**
		 * Decompress assets like .csv or other compressed assets
		 */
		DecompressorResult CommonDecompress(Bytestream& input, Bytestream& output);
	}
}
