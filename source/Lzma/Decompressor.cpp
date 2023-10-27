#include "SupercellCompression/Lzma.h"

#include "exception/MemoryAllocationException.h"
#include "SupercellCompression/exception/lzma/DecompressInitException.h"
#include "SupercellCompression/exception/lzma/DecompressMissingEndMarkException.h"
#include "SupercellCompression/exception/lzma/DecompressCorruptedDataException.h"

namespace sc
{
	namespace Decompressor
	{
		Lzma::Lzma(Byte header[LZMA_PROPS_SIZE], const UInt64 unpackedSize) : m_unpacked_size(unpackedSize)
		{
			LzmaDec_Construct(&m_context);
			SRes res = LzmaDec_Allocate(&m_context, header, LZMA_PROPS_SIZE, &LzAllocObj);
			if (res != SZ_OK)
			{
				throw LzmaDecompressInitException();
			}

			LzmaDec_Init(&m_context);

			m_input_buffer = (uint8_t*)malloc(Lzma::Stream_Size);
			if (!m_input_buffer)
			{
				throw MemoryAllocationException(Lzma::Stream_Size);
			}

			m_output_buffer = (uint8_t*)malloc(Lzma::Stream_Size);
			if (!m_output_buffer)
			{
				if (m_input_buffer) { free(m_input_buffer); }
				throw MemoryAllocationException(Lzma::Stream_Size);
			}
		}

		void Lzma::decompress_stream(Stream& input, Stream& output)
		{
			bool has_strict_bound = (m_unpacked_size != UINT32_MAX && m_unpacked_size != UINT64_MAX);

			size_t in_position = 0, input_size = 0, out_position = 0;
			while (true)
			{
				if (in_position == input_size)
				{
					input_size = Lzma::Stream_Size;
					input.read(m_input_buffer, input_size);
					in_position = 0;
				}
				{
					SRes res;
					size_t in_processed = input_size - in_position;
					size_t out_processed = Lzma::Stream_Size - out_position;
					ELzmaFinishMode finishMode = LZMA_FINISH_ANY;
					ELzmaStatus status;
					if (has_strict_bound && out_processed > m_unpacked_size)
					{
						out_processed = m_unpacked_size;
						finishMode = LZMA_FINISH_END;
					}

					res = LzmaDec_DecodeToBuf(&m_context, m_output_buffer + out_position, &out_processed,
						m_input_buffer + in_position, &in_processed, finishMode, &status);
					in_position += in_processed;
					out_position += out_processed;
					m_unpacked_size -= out_processed;

					if (output.write(m_output_buffer, out_position) != out_position || res != SZ_OK)
						throw LzmaCorruptedDataException();

					out_position = 0;

					if (has_strict_bound && m_unpacked_size == 0)
						// Decompression Success
						return;

					if (in_processed == 0 && out_processed == 0)
					{
						if (has_strict_bound || status != LZMA_STATUS_FINISHED_WITH_MARK)
							throw LzmaMissingEndMarkException();

						// Decompression Success
						return;
					}
				}
			}
		};

		Lzma::~Lzma()
		{
			LzmaDec_Free(&m_context, &LzAllocObj);

			if (m_input_buffer)
			{
				free(m_input_buffer);
			}

			if (m_output_buffer)
			{
				free(m_output_buffer);
			}
		}
	}
}