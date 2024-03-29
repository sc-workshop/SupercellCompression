#include "SupercellCompression/Lzham/Decompressor.h"

#include "SupercellCompression/Lzham/lzham_config.h"

#include "exception/MemoryAllocationException.h"
#include "SupercellCompression/exception/Lzham.h"
#include "memory/alloc.h"

namespace sc
{
	namespace Decompressor
	{
		Lzham::Props::Props()
		{
			struct_size = sizeof(lzham_decompress_params);
		}

		Lzham::Lzham(Props& props)
		{
			m_state = lzham_decompress_init((lzham_decompress_params*)&props);
			if (!m_state)
			{
				throw LzhamDecompressInitException();
			}

			m_unpacked_length = props.unpacked_length;

			m_input_buffer = memalloc(Lzham::Stream_Size);
			m_output_buffer = memalloc(Lzham::Stream_Size);
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

			lzham_decompress_deinit(m_state);
		}

		void Lzham::decompress_stream(Stream& input, Stream& output)
		{
			size_t remain_bytes = input.length() - input.position();

			uint32_t buffer_size = 0, buffer_offset = 0;
			lzham_decompress_status_t status;
			while (true)
			{
				if (buffer_offset == buffer_size)
				{
					buffer_size = static_cast<uint32_t>(Lzham::Stream_Size < remain_bytes ? Lzham::Stream_Size : remain_bytes);
					input.read(m_input_buffer, buffer_size);

					remain_bytes -= buffer_size;

					buffer_offset = 0;
				}

				uint8_t* input_bytes = &m_input_buffer[buffer_offset];
				size_t input_bytes_length = buffer_size - buffer_offset;
				size_t output_bytes_length = Lzham::Stream_Size;

				status = lzham_decompress(m_state, input_bytes, &input_bytes_length, m_output_buffer, &output_bytes_length, remain_bytes == 0);

				if (input_bytes_length)
				{
					buffer_offset += (uint32_t)input_bytes_length;
				}

				if (output_bytes_length)
				{
					output.write(m_output_buffer, static_cast<uint32_t>(output_bytes_length));

					if (output_bytes_length > m_unpacked_length)
					{
						break;
					}
					m_unpacked_length -= output_bytes_length;
				}

				if (status >= LZHAM_DECOMP_STATUS_FIRST_SUCCESS_OR_FAILURE_CODE)
					break;
			}
		}
	}
}