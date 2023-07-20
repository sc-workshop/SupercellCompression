#include "SupercellCompression/Compressor.h"

#include "backend/LzmaCompression.h"
#include "backend/ZstdCompression.h"
#include "backend/LzhamCompression.h"

#include "SupercellBytestream.h"

#include "caching/md5.h"

namespace sc
{
	namespace Compressor
	{
		CompressorResult Compress(const fs::path& input, const fs::path& output, CompressorContext context)
		{
			ReadFileStream inputStream(input);
			WriteFileStream outputStream(output);

			CompressorResult result = Compress(inputStream, outputStream, context);

			inputStream.close();
			outputStream.close();

			return result;
		}

		CompressorResult Compress(Bytestream& input, Bytestream& output, CompressorContext context)
		{
			output.writeUInt16BE(0x5343);

			if (!context.metadata.empty())
			{
				output.writeUInt32BE(4);
			}

			switch (context.signature)
			{
			case sc::CompressionSignature::LZMA:
			case sc::CompressionSignature::LZHAM:
				output.writeUInt32BE(1);
				break;
			case sc::CompressionSignature::ZSTD:
				output.writeUInt32BE(3);
				break;
			default:
				break;
			}

			md5 hashCtx;
			uint8_t hash[16];

			uint8_t* buffer = new uint8_t[input.size()]();
			input.seek(0);
			input.read(buffer, input.size());
			input.seek(0);

			hashCtx.update(buffer, input.size());
			hashCtx.final(hash);

			output.writeUInt32BE(16);
			output.write(&hash, 16);

			CompressorResult result = CommonCompress(input, output, context);

			if (!context.metadata.empty())
			{
				output.write("START", 6);
				output.write(context.metadata.data(), context.metadata.size());
			}

			return result;
		}

		CompressorResult CommonCompress(Bytestream& input, Bytestream& output, CompressorContext context)
		{
			switch (context.signature)
			{
			case CompressionSignature::LZMA:
				return LZMA::Compress(input, output, context.threadCount);

			case CompressionSignature::LZHAM:
				return LZHAM::Compress(input, output, context.threadCount);

			case CompressionSignature::ZSTD:
				return ZSTD::Compress(input, output, context.threadCount);

			default:
				std::vector<uint8_t> dataBuffer(input.size());
				input.read(dataBuffer.data(), dataBuffer.size());
				output.write(dataBuffer.data(), dataBuffer.size());
				return CompressorResult::COMPRESSION_SUCCES;
				break;
			}
		}
	}
}