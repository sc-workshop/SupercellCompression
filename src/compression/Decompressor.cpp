#include "SupercellCompression/Decompressor.h"

#include <iostream>

#include "caching/cache.h"

#include "backend/LzmaCompression.h"
#include "backend/LzhamCompression.h"
#include "backend/ZstdCompression.h"

namespace sc {
	DecompressorResult CommonDecompress(Bytestream& inStream, Bytestream& outStream, CompressionSignature signature) {
		switch (signature)
		{
		case CompressionSignature::LZMA:
			return LZMA::Decompress(inStream, outStream);
			break;

		case CompressionSignature::LZHAM:
			return LZHAM::Decompress(inStream, outStream);
			break;

		case CompressionSignature::ZSTD:
			return ZSTD::Decompress(inStream, outStream);
			break;

		default:
			std::vector<uint8_t> dataBuffer(inStream.size());
			inStream.seek(0);
			inStream.read(dataBuffer.data(), dataBuffer.size());
			outStream.write(dataBuffer.data(), dataBuffer.size());
			return DecompressorResult::DECOMPRESSION_SUCCES;
		}
	}

	DecompressorResult GetHeader(sc::Bytestream& inStream, sc::CompressionSignature& signature, std::vector<uint8_t>& hash) {
		uint16_t magic = inStream.readUInt16BE();

		if (magic != 0x5343) {
#ifdef _DEBUG
			signature = sc::CompressionSignature::NONE;
			return DecompressorResult::DECOMPRESSION_SUCCES;
#else
			return DecompressorResult::CORRUPTED_HEADER_ERROR;
#endif
		}

		uint32_t version = inStream.readUInt32BE();
		if (version == 4) {
			version = inStream.readUInt32BE();
		}

		if (version == 3) {
			signature = CompressionSignature::ZSTD;
		}

		uint32_t hashSize = inStream.readUInt32BE();
		hash = std::vector<uint8_t>(hashSize);
		inStream.read(hash.data(), hashSize);

		if (version == 1)
		{
			uint32_t compressMagic = inStream.readUInt32();

			// SCLZ
			if (compressMagic == 0x5A4C4353)
			{
				signature = CompressionSignature::LZHAM;
			}

			// LZMA
			else
			{
				signature = CompressionSignature::LZMA;
			}

			inStream.seek(inStream.tell() - sizeof(compressMagic));
		}

		return DecompressorResult::DECOMPRESSION_SUCCES;
	}
}

namespace sc
{
	namespace Decompressor {
		DecompressorResult Decompress(const fs::path& filepath, fs::path& outFilepath)
		{
			ReadFileStream input(filepath);
			outFilepath = SwfCache::GetTempDirectory(filepath);

			/* Header parsing */
			CompressionSignature signature;
			std::vector<uint8_t> hash;
			DecompressorResult result = GetHeader(input, signature, hash);

			if (result != DecompressorResult::DECOMPRESSION_SUCCES)
				return result;

			bool isCached = SwfCache::IsFileCached(filepath, hash, input.size());
			if (isCached)
			{
				return DecompressorResult::DECOMPRESSION_SUCCES;
			}

			WriteFileStream output(outFilepath);

			result = CommonDecompress(input, output, signature);

			input.close();
			output.close();

			if (!isCached)
			{
				SwfCache::WriteCacheInfo(filepath, hash, input.size());
			}

			return result;
		}

		DecompressorResult Decompress(Bytestream& input, Bytestream& output)
		{
			CompressionSignature signature;
			std::vector<uint8_t> hash;

			DecompressorResult result = GetHeader(input, signature, hash);

			if (result != DecompressorResult::DECOMPRESSION_SUCCES)
				return result;

			return CommonDecompress(input, output, signature);
		}

		DecompressorResult Decompressor::CommonDecompress(Bytestream& input, Bytestream& output)
		{
			input.seek(0);
			uint32_t magic = input.readUInt32();
			input.read(&magic, sizeof(magic));
			input.seek(0);

			if (magic == 0x3A676953) {
				input.skip(64);
				magic = input.readUInt32();
			}

			CompressionSignature signature = getSignature(magic);

			return CommonDecompress(input, output, signature);
		}
	}
}