#include "LzmaCompression.h"

#include <Alloc.h>
#include <LzmaDec.h>
#include <LzmaEnc.h>

static const size_t LZMA_DECOMPRESS_BUF_SIZE = 1 << 16;
static const size_t LZMA_COMPRESS_BUF_SIZE = 1 << 24;

static const size_t SMALL_FILE_SIZE = 1 << 28;
static const uint32_t LZMA_COMPRESS_DICT_SIZE = 262144;

struct CSeqInStreamWrap
{
	ISeqInStream vt;
	sc::BytestreamBase* inStream;
};

struct CSeqOutStreamWrap
{
	ISeqOutStream vt;
	sc::BytestreamBase* outStream;
};

static SRes LzmaStreamRead(const ISeqInStream* p, void* data, size_t* size)
{
	CSeqInStreamWrap* wrap = CONTAINER_FROM_VTBL(p, CSeqInStreamWrap, vt);
	size_t bufferReadSize = (*size < LZMA_COMPRESS_BUF_SIZE) ? *size : LZMA_COMPRESS_BUF_SIZE;
	size_t readSize = wrap->inStream->read(data, bufferReadSize);

	*size = readSize;
	return SZ_OK;
}

static size_t LzmaStreamWrite(const ISeqOutStream* p, const void* buf, size_t size)
{
	auto* wrap = CONTAINER_FROM_VTBL(p, CSeqOutStreamWrap, vt);
	return wrap->outStream->write((void*)buf, size);
}

namespace sc {
	DecompressorResult DecompressStream(CLzmaDec* state, SizeT unpackedSize, BytestreamBase& input, BytestreamBase& output)
	{
		int hasBound = (unpackedSize != (UInt32)(Int32)-1);

		uint8_t inBuffer[LZMA_DECOMPRESS_BUF_SIZE];
		uint8_t outBuffer[LZMA_DECOMPRESS_BUF_SIZE];

		LzmaDec_Init(state);

		size_t inPos = 0, inSize = 0, outPos = 0;
		while (true)
		{
			if (inPos == inSize)
			{
				inSize = LZMA_DECOMPRESS_BUF_SIZE;
				input.read(&inBuffer, inSize);
				inPos = 0;
			}
			{
				SRes res;
				size_t inProcessed = inSize - inPos;
				size_t outProcessed = LZMA_DECOMPRESS_BUF_SIZE - outPos;
				ELzmaFinishMode finishMode = LZMA_FINISH_ANY;
				ELzmaStatus status;
				if (hasBound && outProcessed > unpackedSize)
				{
					outProcessed = (size_t)unpackedSize;
					finishMode = LZMA_FINISH_END;
				}

				res = LzmaDec_DecodeToBuf(state, outBuffer + outPos, &outProcessed,
					inBuffer + inPos, &inProcessed, finishMode, &status);
				inPos += inProcessed;
				outPos += outProcessed;
				unpackedSize -= outProcessed;

				if (output.write(&outBuffer, outPos) != outPos || res != SZ_OK)
					return sc::DecompressorResult::LZMA_CORRUPTED_DATA_ERROR;

				outPos = 0;

				if (hasBound && unpackedSize == 0)
					DecompressorResult::DECOMPRESSION_SUCCES;

				if (inProcessed == 0 && outProcessed == 0)
				{
					if (hasBound || status != LZMA_STATUS_FINISHED_WITH_MARK)
						return sc::DecompressorResult::LZMA_MISSING_END_MARKER_ERROR;

					return sc::DecompressorResult::DECOMPRESSION_SUCCES;
				}
			}
		}
	}
}

namespace sc
{
	namespace LZMA {
		DecompressorResult Decompress(BytestreamBase& input, BytestreamBase& output)
		{
			CLzmaDec state;
			Byte header[LZMA_PROPS_SIZE];
			input.read(header, LZMA_PROPS_SIZE);

			unsigned int unpackSize = 0;
			input.read(&unpackSize, sizeof(unpackSize));

			LzmaDec_Construct(&state);
			LzmaDec_Allocate(&state, header, LZMA_PROPS_SIZE, &g_Alloc);

			DecompressorResult result = DecompressStream(&state, unpackSize, input, output);

			LzmaDec_Free(&state, &g_Alloc);

			return result;
		}

		CompressorResult Compress(BytestreamBase& input, BytestreamBase& output, uint32_t threads)
		{
			CLzmaEncHandle enc;
			SRes res;
			CLzmaEncProps props;
			enc = LzmaEnc_Create(&g_Alloc);
			if (enc == 0) return CompressorResult::LZMA_STREAM_INIT_ERROR;

			LzmaEncProps_Init(&props);
			props.level = 6;
			props.pb = 2;
			props.lc = 3;
			props.lp = 0;
			props.numThreads = threads;

			props.dictSize = LZMA_COMPRESS_DICT_SIZE;

			if (input.size() > SMALL_FILE_SIZE)
				props.lc = 4;

			LzmaEncProps_Normalize(&props);
			res = LzmaEnc_SetProps(enc, &props);

			if (res == SZ_OK)
			{
				uint8_t header[LZMA_PROPS_SIZE];
				size_t headerSize = LZMA_PROPS_SIZE;
				res = LzmaEnc_WriteProperties(enc, header, &headerSize);

				output.write(&header, headerSize);

				uint32_t outStreamSize = static_cast<uint32_t>(input.size());

				output.write(&outStreamSize, sizeof(outStreamSize));

				// Read stream wrap
				CSeqInStreamWrap inWrap = {};
				inWrap.vt.Read = LzmaStreamRead;
				inWrap.inStream = &input;

				// Write stream wrap
				CSeqOutStreamWrap outWrap = {};
				outWrap.vt.Write = LzmaStreamWrite;
				outWrap.outStream = &output;
				LzmaEnc_Encode(enc, &outWrap.vt, &inWrap.vt, nullptr, &g_Alloc, &g_Alloc);
			}

			LzmaEnc_Destroy(enc, &g_Alloc, &g_Alloc);

			return CompressorResult::COMPRESSION_SUCCES;
		}
	}
}