#include "SupercellCompression/Lzma.h"

#include "SupercellCompression/exception/lzma/CompressInitException.h"
#include "SupercellCompression/exception/lzma/CompressPropsException.h"

static const size_t Stream_Size = (4 * 1024 * 1024); // 4MB
static  SizeT LzmaHeaderLength = LZMA_PROPS_SIZE;

struct CSeqInStreamWrap
{
	ISeqInStream vt;
	sc::Stream* input;
};

struct CSeqOutStreamWrap
{
	ISeqOutStream vt;
	sc::Stream* output;
};

static SRes LzmaStreamRead(const ISeqInStream* p, void* data, size_t* size)
{
	CSeqInStreamWrap* wrap = CONTAINER_FROM_VTBL(p, CSeqInStreamWrap, vt);
	size_t bufferReadSize = (*size < Stream_Size) ? *size : Stream_Size;
	size_t readSize = wrap->input->read(data, bufferReadSize);

	*size = readSize;
	return SZ_OK;
};

static size_t LzmaStreamWrite(const ISeqOutStream* p, const void* buf, size_t size)
{
	auto* wrap = CONTAINER_FROM_VTBL(p, CSeqOutStreamWrap, vt);
	return wrap->output->write((void*)buf, size);
};

namespace sc
{
	namespace Compressor
	{
		Lzma::Lzma(LzmaProps& props)
		{
			m_context = LzmaEnc_Create(&g_Alloc);
			if (!m_context)
			{
				throw LzmaCompressInitException();
			}

			SRes res;
			res = LzmaEnc_SetProps(m_context, &props);
			if (res != SZ_OK)
			{
				throw LzmaCompressPropsException();
			}
			m_use_long_unpacked_data = props.use_long_unpacked_length;
		}

		void Lzma::compress_stream(Stream& input, Stream& output)
		{
			Byte header[LZMA_PROPS_SIZE];
			LzmaEnc_WriteProperties(m_context, (Byte*)header, &LzmaHeaderLength);
			output.write(header, LZMA_PROPS_SIZE);

			size_t file_size = input.length() - input.position();
			if (m_use_long_unpacked_data)
			{
				output.write_unsigned_long(file_size);
			}
			else
			{
				output.write_unsigned_int(static_cast<uint32_t>(file_size));
			}

			CSeqInStreamWrap inWrap = {};
			inWrap.vt.Read = LzmaStreamRead;
			inWrap.input = &input;

			// Write stream wrap
			CSeqOutStreamWrap outWrap = {};
			outWrap.vt.Write = LzmaStreamWrite;
			outWrap.output = &output;
			LzmaEnc_Encode(m_context, &outWrap.vt, &inWrap.vt, nullptr, &g_Alloc, &g_Alloc);
		}

		Lzma::~Lzma()
		{
			LzmaEnc_Destroy(m_context, &g_Alloc, &g_Alloc);
		}
	}
}