#pragma once

#include "SupercellBytestream.h"

#include <filesystem>
#include <functional>
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
		typedef std::function<void(const char*, const char*, uint32_t)> MetadataCallback;

		/**
		 * Decompress file and then store it in cache, without need to decompress in the future.
		 */
		DecompressorResult Decompress(const fs::path& filepath, fs::path& outFilepath, MetadataCallback callback = {});

		/**
		 * Decompress file from stream.
		 */
		DecompressorResult Decompress(BufferStream& input, Bytestream& output, MetadataCallback callback = {});

		/**
		 * Decompress assets like .csv or other compressed assets
		 */
		DecompressorResult CommonDecompress(BufferStream& input, Bytestream& output);
	}
}
