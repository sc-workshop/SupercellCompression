#include "compression/backend/lzma/lzma_decompressor.h"

#include <core/exception/exception.h>

#include <Alloc.h>
#include <LzmaDec.h>

using namespace wk;

namespace sc
{
	struct LzmaDecompressContext : public CLzmaDec {};

	LzmaDecompressor::LzmaDecompressor(uint8_t header[lzma::PROPS_SIZE], const uint64_t unpackedSize) : m_unpacked_size(unpackedSize)
	{
		m_context = new LzmaDecompressContext();
		LzmaDec_Construct(m_context);

		SRes res = LzmaDec_Allocate(m_context, header, LZMA_PROPS_SIZE, (ISzAllocPtr)&LzmaAlloc);
		if (res != SZ_OK)
		{
			throw Exception("Failed to allocate LZMA compress context with code %d", res);
		}

		LzmaDec_Init(m_context);

		m_input_buffer = Memory::allocate(LzmaDecompressor::s_stream_size);
		m_output_buffer = Memory::allocate(LzmaDecompressor::s_stream_size);
	}

	void LzmaDecompressor::decompress(Stream& input, Stream& output)
	{
		bool has_strict_bound = (m_unpacked_size != SIZE_MAX / 2) && (m_unpacked_size != SIZE_MAX);

		std::size_t in_position = 0, input_size = 0, out_position = 0;
		while (true)
		{
			if (in_position == input_size)
			{
				input_size = LzmaDecompressor::s_stream_size;
				input.read(m_input_buffer, input_size);
				in_position = 0;
			}
			{
				SRes res;
				std::size_t in_processed = input_size - in_position;
				std::size_t out_processed = LzmaDecompressor::s_stream_size - out_position;
				ELzmaFinishMode finishMode = LZMA_FINISH_ANY;
				ELzmaStatus status;
				if (has_strict_bound && out_processed > m_unpacked_size)
				{
					out_processed = m_unpacked_size;
					finishMode = LZMA_FINISH_END;
				}

				res = LzmaDec_DecodeToBuf(m_context, m_output_buffer + out_position, &out_processed, m_input_buffer + in_position, &in_processed, finishMode, &status);
				in_position += in_processed;
				out_position += out_processed;
				m_unpacked_size -= out_processed;

				if (output.write(m_output_buffer, out_position) != out_position || res != SZ_OK)
					throw Exception("LZMA missing end marker!");

				out_position = 0;

				if (has_strict_bound && m_unpacked_size == 0)
					// Decompression Success
					return;

				if (in_processed == 0 && out_processed == 0)
				{
					if (has_strict_bound || status != LZMA_STATUS_FINISHED_WITH_MARK)
						throw Exception("LZMA missing end marker!");

					// Decompression Success
					return;
				}
			}
		}
	};

	LzmaDecompressor::~LzmaDecompressor()
	{
		LzmaDec_Free(m_context, (ISzAllocPtr)&LzmaAlloc);

		if (m_input_buffer)
		{
			Memory::free(m_input_buffer);
		}

		if (m_output_buffer)
		{
			Memory::free(m_output_buffer);
		}

		delete m_context;
	}
}