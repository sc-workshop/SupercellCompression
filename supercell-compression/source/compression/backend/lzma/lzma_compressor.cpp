#include "compression/backend/lzma/lzma_compressor.h"

#include <core/exception/exception.h>

#include <Alloc.h>
#include <LzmaEnc.h>

const std::size_t stream_size = (4 * 1024 * 1024); // 4MB

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

static SRes LzmaStreamRead(const ISeqInStream* p, void* data, std::size_t* size)
{
	CSeqInStreamWrap* wrap = CONTAINER_FROM_VTBL(p, CSeqInStreamWrap, vt);
	std::size_t bufferReadSize = (*size < stream_size) ? *size : stream_size;
	std::size_t readSize = wrap->input->read(data, bufferReadSize);

	*size = readSize;
	return SZ_OK;
};

static std::size_t LzmaStreamWrite(const ISeqOutStream* p, const void* buf, std::size_t size)
{
	auto* wrap = CONTAINER_FROM_VTBL(p, CSeqOutStreamWrap, vt);
	return wrap->output->write((void*)buf, size);
};

namespace sc
{
	LzmaCompressor::LzmaCompressor(Props& props)
	{
		m_context = LzmaEnc_Create((ISzAllocPtr)&LzmaAlloc);
		if (!m_context)
		{
			throw Exception("Failed to initialize LZMA compression!");
		}

		SRes res;
		res = LzmaEnc_SetProps(m_context, (CLzmaEncProps*)&props);
		if (res != SZ_OK)
		{
			throw Exception("Failed to initialize LZMA compression!");
		}

		m_use_long_unpacked_data = props.use_long_unpacked_length;
	}

	void LzmaCompressor::compress(Stream& input, Stream& output)
	{
		SizeT header_length = lzma::PROPS_SIZE;
		Byte header[lzma::PROPS_SIZE];
		LzmaEnc_WriteProperties(m_context, (Byte*)&header, (SizeT*)&header_length);
		output.write(header, header_length);

		std::size_t file_size = input.length() - input.position();
		if (m_use_long_unpacked_data)
		{
			output.write_unsigned_long(file_size);
		}
		else
		{
			output.write_unsigned_int(static_cast<std::uint32_t>(file_size));
		}

		CSeqInStreamWrap inWrap;
		inWrap.vt.Read = LzmaStreamRead;
		inWrap.input = &input;

		CSeqOutStreamWrap outWrap;
		outWrap.vt.Write = LzmaStreamWrite;
		outWrap.output = &output;

		LzmaEnc_Encode(m_context, &outWrap.vt, &inWrap.vt, nullptr, (ISzAllocPtr)&LzmaAlloc, (ISzAllocPtr)&LzmaAlloc);
	}

	LzmaCompressor::~LzmaCompressor()
	{
		LzmaEnc_Destroy(m_context, (ISzAllocPtr)&LzmaAlloc, (ISzAllocPtr)&LzmaAlloc);
	}
}
