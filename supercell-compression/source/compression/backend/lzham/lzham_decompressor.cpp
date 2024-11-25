#include "compression/backend/lzham/lzham_decompressor.h"

#include "compression/backend/lzham/lzham_config.h"

#include <core/memory/memory.h>

using namespace wk;

namespace sc
{
	LzhamDecompressor::Props::Props()
	{
		struct_size = sizeof(lzham_decompress_params);
	}

	LzhamDecompressor::LzhamDecompressor(Props& props)
	{
		m_state = lzham_decompress_init((lzham_decompress_params*)&props);
		if (!m_state)
		{
			throw Exception("Failed to initialize LZHAM decompression!");
		}

		m_unpacked_length = props.unpacked_length;

		m_input_buffer = Memory::allocate(LzhamDecompressor::sm_stream_size);
		m_output_buffer = Memory::allocate(LzhamDecompressor::sm_stream_size);
	}

	LzhamDecompressor::~LzhamDecompressor()
	{
		if (m_input_buffer)
		{
			Memory::free(m_input_buffer);
		}

		if (m_output_buffer)
		{
			Memory::free(m_output_buffer);
		}

		lzham_decompress_deinit(m_state);
	}

	void LzhamDecompressor::decompress(Stream& input, Stream& output)
	{
		std::size_t remain_bytes = input.length() - input.position();

		std::uint32_t buffer_size = 0, buffer_offset = 0;
		lzham_decompress_status_t status = LZHAM_DECOMP_STATUS_NOT_FINISHED;
		while (true)
		{
			if (buffer_offset == buffer_size)
			{
				buffer_size = static_cast<uint32_t>(LzhamDecompressor::sm_stream_size < remain_bytes ? LzhamDecompressor::sm_stream_size : remain_bytes);
				input.read(m_input_buffer, buffer_size);

				remain_bytes -= buffer_size;

				buffer_offset = 0;
			}

			std::uint8_t* input_bytes = &m_input_buffer[buffer_offset];
			std::size_t input_bytes_length = buffer_size - buffer_offset;
			std::size_t output_bytes_length = LzhamDecompressor::sm_stream_size;

			status = lzham_decompress(m_state, input_bytes, &input_bytes_length, m_output_buffer, &output_bytes_length, remain_bytes == 0);

			if (input_bytes_length)
			{
				buffer_offset += (std::uint32_t)input_bytes_length;
			}

			if (output_bytes_length)
			{
				output.write(m_output_buffer, static_cast<std::uint32_t>(output_bytes_length));

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