#include "SupercellCompression/Zstd.h"

#include <zstd.h>

#include "exception/MemoryAllocationException.h"
#include "SupercellCompression/exception/Zstd.h"
#include "memory/alloc.h"

namespace sc
{
	namespace Decompressor
	{
		Zstd::Zstd() : Input_Buffer_Size(ZSTD_DStreamInSize()), Output_Buffer_Size(ZSTD_DStreamOutSize())
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

			m_input_buffer = memalloc(Input_Buffer_Size);
			m_output_buffer = memalloc(Output_Buffer_Size);
		}

		void Zstd::decompress_stream(Stream& input, Stream& output)
		{
			uint64_t unpacked_size = UINT64_MAX;

			{
				size_t position = input.position();
				uint8_t frame_header[18];
				input.read(frame_header, sizeof(frame_header));
				unpacked_size = ZSTD_getFrameContentSize(&frame_header, sizeof(frame_header));

				if (unpacked_size == ZSTD_CONTENTSIZE_ERROR)
				{
					unpacked_size = UINT64_MAX;
				}

				input.seek(position);
			}

			ZSTD_inBuffer input_buffer;
			input_buffer.src = m_input_buffer;
			input_buffer.size = Input_Buffer_Size;

			ZSTD_outBuffer output_buffer;
			output_buffer.dst = m_output_buffer;
			output_buffer.size = Output_Buffer_Size;

			size_t chunk_size = 0;
			size_t total_size = 0;

			while (total_size < unpacked_size)
			{
				chunk_size = input.read(m_input_buffer, Input_Buffer_Size);
				if (!chunk_size) {
					if (unpacked_size > total_size)
					{
						throw ZstdCorruptedDecompressException();
					}

					break;
				}

				input_buffer.size = chunk_size;
				input_buffer.pos = 0;

				while (input_buffer.pos < input_buffer.size && total_size < unpacked_size)
				{
					output_buffer.pos = 0;

					size_t result = ZSTD_decompressStream(m_context, &output_buffer, &input_buffer);

					if (ZSTD_isError(result)) {
						throw ZstdCorruptedDecompressException();
					}

					output.write(m_output_buffer, output_buffer.pos);
					total_size += output_buffer.pos;

					if (result == 0) break;
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