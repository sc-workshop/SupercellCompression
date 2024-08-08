#include "compression/backend/lzham/lzham_compressor.h"

#include "compression/backend/lzham/lzham_config.h"

#include "core/exception/exception.h"
#include "core/memory/memory.h"

#define min(a, b) (((a) < (b)) ? (a) : (b))

namespace sc
{
	void LzhamCompressor::write(Stream& input, Stream& output, Props& props)
	{
		output.write(&lzham::file_identifier, sizeof(lzham::file_identifier));
		output.write_unsigned_int(props.dict_size_log2);
		output.write_unsigned_long(input.length() - input.position());

		LzhamCompressor context(props);
		context.compress(input, output);
	}

	LzhamCompressor::LzhamCompressor(Props& props)
	{
		m_state = lzham_compress_init((lzham_compress_params*)&props);
		if (!m_state)
		{
			throw Exception("Failed to initialize LZHAM compression!");
		}

		m_input_buffer = Memory::allocate(LzhamCompressor::sm_stream_size);
		m_output_buffer = Memory::allocate(LzhamCompressor::sm_stream_size);
	}

	LzhamCompressor::~LzhamCompressor()
	{
		if (m_input_buffer)
		{
			Memory::free(m_input_buffer);
		}

		if (m_output_buffer)
		{
			Memory::free(m_output_buffer);
		}

		lzham_compress_deinit(m_state);
	}

	void LzhamCompressor::compress(Stream& input, Stream& output)
	{
		std::uint32_t input_buffer_position = 0;
		std::uint32_t input_buffer_offset = 0;

		std::size_t remain_bytes = input.length() - input.position();

		lzham_compress_status_t status = LZHAM_COMP_STATUS_FAILED;
		while (true)
		{
			if (input_buffer_offset == input_buffer_position)
			{
				input_buffer_position = static_cast<uint32_t>(min(LzhamCompressor::sm_stream_size, remain_bytes));
				if (input.read(m_input_buffer, input_buffer_position) != input_buffer_position)
				{
					throw Exception("Corrupted LZHAM compressed data!");
				}

				remain_bytes -= input_buffer_position;

				input_buffer_offset = 0;
			}

			std::uint8_t* input_bytes = &m_input_buffer[input_buffer_offset];
			std::size_t input_bytes_lengh = input_buffer_position - input_buffer_offset;
			std::size_t output_bytes_length = LzhamCompressor::sm_stream_size;

			status = lzham_compress(m_state, input_bytes, &input_bytes_lengh, m_output_buffer, &output_bytes_length, remain_bytes == 0);

			if (input_bytes_lengh)
			{
				input_buffer_offset += (uint32_t)input_bytes_lengh;
			}

			if (output_bytes_length)
			{
				output.write(m_output_buffer, output_bytes_length);
			}

			if (status >= LZHAM_COMP_STATUS_FIRST_SUCCESS_OR_FAILURE_CODE)
				break;
		}
	}
}