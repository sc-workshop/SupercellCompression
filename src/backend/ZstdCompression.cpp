#include "ZstdCompression.h"

#include <zstd.h>

namespace sc {
	namespace ZSTD
	{
		DecompressorResult Decompress(BytestreamBase& input, BytestreamBase& output) {
			const size_t inBufferSize = ZSTD_DStreamInSize();
			const size_t outBufferSize = ZSTD_DStreamOutSize();

			void* inBuffer = malloc(inBufferSize);
			void* outBuffer = malloc(outBufferSize);

			if (!inBuffer || !outBuffer) {
				return DecompressorResult::ALLOC_ERROR;
			};

			ZSTD_DStream* const dctx = ZSTD_createDStream();

			if (!dctx)
			{
				return DecompressorResult::ZSTD_STREAM_INIT_ERROR;
			}

			size_t dRes = ZSTD_initDStream(dctx);

			if (ZSTD_isError(dRes))
			{
				ZSTD_freeDStream(dctx);
				return DecompressorResult::ZSTD_STREAM_INIT_ERROR;
			}

			uint32_t pos = input.tell();
			size_t bufferSize = input.size() - pos;

			void* buffer = input.data();
			if (buffer == nullptr) {
				buffer = malloc(bufferSize);

				if (buffer == NULL) {
					return DecompressorResult::ALLOC_ERROR;
				}

				input.read(buffer, bufferSize);
			};

			uint64_t unpackedSize = ZSTD_getDecompressedSize(buffer, bufferSize);
			if (unpackedSize == 0) {
				unpackedSize = UINT32_MAX;
			}

			if (input.data() == nullptr) {
				free(buffer);
				input.seek(pos);
			}

			ZSTD_inBuffer zInBuffer;
			ZSTD_outBuffer zOutBuffer;
			size_t chunkSize = 0;
			size_t totalBytes = 0;

			while (totalBytes < unpackedSize) {
				chunkSize = input.read(inBuffer, inBufferSize);
				if (!chunkSize) {
					break;
				}

				zInBuffer.src = inBuffer;
				zInBuffer.size = chunkSize;
				zInBuffer.pos = 0;

				while (zInBuffer.pos < zInBuffer.size && totalBytes < unpackedSize) {
					zOutBuffer.dst = outBuffer;
					zOutBuffer.size = outBufferSize;
					zOutBuffer.pos = 0;
					dRes = ZSTD_decompressStream(dctx, &zOutBuffer, &zInBuffer);
					if (ZSTD_isError(dRes)) {
						return DecompressorResult::ZSTD_CORRUPTED_DATA_ERROR;
					}
					output.write(outBuffer, zOutBuffer.pos);
					totalBytes += zOutBuffer.pos;
				}
			}

			ZSTD_freeDStream(dctx);
			free(inBuffer);
			free(outBuffer);

			return DecompressorResult::DECOMPRESSION_SUCCES;
		}

		CompressorResult Compress(BytestreamBase& input, BytestreamBase& output, uint32_t theards)
		{
			size_t const buffInSize = ZSTD_CStreamInSize();
			size_t const buffOutSize = ZSTD_CStreamOutSize();

			void* buffIn = malloc(buffInSize);
			void* buffOut = malloc(buffOutSize);

			if (!buffIn || !buffOut)
				return CompressorResult::ALLOC_ERROR;

			ZSTD_CCtx* const cctx = ZSTD_createCCtx();
			if (cctx == NULL)
				return CompressorResult::ZSTD_STREAM_INIT_ERROR;

			ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 16);
			ZSTD_CCtx_setParameter(cctx, ZSTD_c_checksumFlag, 0);

			ZSTD_CCtx_setParameter(cctx, ZSTD_c_contentSizeFlag, 1);
			ZSTD_CCtx_setPledgedSrcSize(cctx, input.size());

			ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, theards);

			size_t const remainBytes = buffInSize;
			while (true) {
				size_t byteCount = input.read(buffIn, remainBytes);

				int const lastChunk = (byteCount < remainBytes);
				ZSTD_EndDirective const mode = lastChunk ? ZSTD_e_end : ZSTD_e_continue;
				ZSTD_inBuffer zInput = { buffIn, byteCount, 0 };
				int finished = 0;
				while (!finished) {
					ZSTD_outBuffer zOutput = { buffOut, buffOutSize, 0 };
					size_t const remaining = ZSTD_compressStream2(cctx, &zOutput, &zInput, mode);
					output.write(buffOut, zOutput.pos);
					finished = lastChunk ? (remaining == 0) : (zInput.pos == zInput.size);
				};
				if (zInput.pos != zInput.size)
					return CompressorResult::ZSTD_CORRUPTED_DATA_ERROR;

				if (lastChunk) {
					break;
				}
			}

			ZSTD_freeCCtx(cctx);
			free(buffIn);
			free(buffOut);

			return CompressorResult::COMPRESSION_SUCCES;
		}
	}
}