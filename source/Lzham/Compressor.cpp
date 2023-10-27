#include "SupercellCompression/Lzham/Compressor.h"

#include "memory/alloc.h"
#include "SupercellCompression/exception/Lzham/CompressInitException.h"
#include "SupercellCompression/exception/Lzham/CompressCorruptedDataException.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))

namespace sc
{
	namespace Compressor
	{
		Lzham::Lzham(LzhamCompressProps& props)
		{
			m_state = lzham_compress_init(&props);
			if (!m_state)
			{
				throw LzhamCompressInitException();
			}

			m_input_buffer = (uint8_t*)malloc(Lzham::Stream_Size);
			m_output_buffer = (uint8_t*)malloc(Lzham::Stream_Size);

			if (!m_input_buffer || m_output_buffer)
			{
				if (m_input_buffer) { free(m_input_buffer); }
				if (m_output_buffer) { free(m_output_buffer); }

				throw MemoryAllocationException(Lzham::Stream_Size);
			}
		}

		Lzham::~Lzham()
		{
			if (m_input_buffer)
			{
				free(m_input_buffer);
			}

			if (m_output_buffer)
			{
				free(m_output_buffer);
			}

			lzham_compress_deinit(m_state);
		}

		void Lzham::compress_stream(Stream& input, Stream& output)
		{
			uint32_t input_buffer_position = 0;
			uint32_t input_buffer_offset = 0;

			size_t remain_bytes = input.length() - input.position();

			lzham_compress_status_t status = LZHAM_COMP_STATUS_FAILED;
			while (true)
			{
				if (input_buffer_offset == input_buffer_position)
				{
					input_buffer_position = static_cast<uint32_t>(min(Lzham::Stream_Size, remain_bytes));
					if (input.read(m_input_buffer, input_buffer_position) != input_buffer_position)
					{
						throw LzhamCompressCorruptedDataException();
					}

					remain_bytes -= input_buffer_position;

					input_buffer_offset = 0;
				}

				uint8_t* input_bytes = &m_input_buffer[input_buffer_offset];
				size_t input_bytes_lengh = input_buffer_position - input_buffer_offset;
				size_t output_bytes_length = Lzham::Stream_Size;

				status = lzham_compress(m_state, input_bytes, &input_bytes_lengh, m_output_buffer, &output_bytes_length, remain_bytes == 0);

				if (input_bytes_lengh)
				{
					input_buffer_offset += (uint32_t)input_bytes_lengh;
				}

				if (output_bytes_length)
				{
					if (output.write(m_output_buffer, output_bytes_length) != output_bytes_length)
					{
						throw LzhamCompressCorruptedDataException();
					}
				}

				if (status >= LZHAM_COMP_STATUS_FIRST_SUCCESS_OR_FAILURE_CODE)
					break;
			}
		}
	}
}