#include "SupercellCompression/Zstd.h"

#include "exception/MemoryAllocationException.h"
#include "SupercellCompression/exception/Zstd/DecompressInitException.h"
#include "SupercellCompression/exception/Zstd/CorruptedDataException.h"

namespace sc
{
	namespace Decompressor
	{
		Zstd::Zstd()
		{
			m_context = ZSTD_createDStream();
			if (!m_context)
			{
				throw ZstdDecompressInitException();
			}

			size_t res = ZSTD_initDStream(m_context);
			if (ZSTD_isError(res))
			{
				ZSTD_freeDStream(m_context);
				throw ZstdDecompressInitException();
			}

			if (!m_input_buffer)
			{
				m_input_buffer = (uint8_t*)malloc(Input_Buffer_Size);
				if (!m_input_buffer)
				{
					throw MemoryAllocationException(Input_Buffer_Size);
				}
			}

			if (!m_output_buffer)
			{
				m_output_buffer = (uint8_t*)malloc(Output_Buffer_Size);
				if (!m_output_buffer)
				{
					throw MemoryAllocationException(Output_Buffer_Size);
				}
			}
		}

		void Zstd::decompress_stream(Stream& input, Stream& output)
		{
			uint64_t unpacked_size = ZSTD_getDecompressedSize(input.data(), input.length());

			ZSTD_inBuffer input_buffer;
			input_buffer.src = m_input_buffer;

			ZSTD_outBuffer output_buffer;
			output_buffer.dst = m_output_buffer;
			output_buffer.size = Output_Buffer_Size;

			size_t chunk_size = 0;
			size_t total_size = 0;

			while (total_size < unpacked_size)
			{
				chunk_size = input.read(m_input_buffer, Input_Buffer_Size);
				if (!chunk_size) {
					break;
				}

				input_buffer.size = chunk_size;
				input_buffer.pos = 0;

				while (input_buffer.pos < input_buffer.size && total_size < unpacked_size)
				{
					output_buffer.pos = 0;

					size_t res = ZSTD_decompressStream(m_context, &output_buffer, &input_buffer);
					if (ZSTD_isError(res)) {
						throw ZstdCorruptedDataException();
					}

					output.write(m_output_buffer, output_buffer.pos);
					total_size += output_buffer.pos;
				}
			}
		}

		Zstd::~Zstd()
		{
			ZSTD_freeDStream(m_context);
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