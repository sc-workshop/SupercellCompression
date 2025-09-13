#include "compression/backend/astc/astc_compressor.h"

#include "core/memory/memory.h"
#include "core/exception/exception.h"
#include "core/parallel/enumerate.h"

#include <astcenc.h>
#include <vector>

using namespace wk;

namespace sc
{
	void ASTCCompressor::write_astc_header(Stream& output, std::uint16_t width, std::uint16_t height, const Props& props)
	{
		output.write(astc::file_identifier, sizeof(astc::file_identifier));

		// x y z blocks
		output.write_unsigned_byte(props.blocks_x);
		output.write_unsigned_byte(props.blocks_y);
		output.write_unsigned_byte(1);

		// '24-bit' integers for width / height
		output.write(&width, 3);
		output.write(&height, 3);

		std::uint32_t z_dimension = 1;
		output.write(&z_dimension, 3);
	}

	ASTCCompressor::ASTCCompressor(Props& props)
	{
		m_threads_count = props.threads_count;
		m_config = new astcenc_config();
		astcenc_error status = astcenc_config_init(
			(astcenc_profile)props.profile,
			props.blocks_x, props.blocks_y, 1,
			float(props.quality), 0, m_config
		);

		if (status != astcenc_error::ASTCENC_SUCCESS)
			throw Exception("Failed to init ASTC with code %d", (int)status);

		status = astcenc_context_alloc(m_config, props.threads_count, &m_context);

		if (status != astcenc_error::ASTCENC_SUCCESS)
			throw Exception("Failed to allocate ASTC context with code %d", (int)status);
	}

	ASTCCompressor::~ASTCCompressor()
	{
		if (m_context)
			astcenc_context_free(m_context);

		if (m_config)
			delete m_config;
	}

	void ASTCCompressor::compress(std::uint16_t width, std::uint16_t height, Image::BasePixelType pixel_type, Stream& input, Stream& output)
	{
		astcenc_swizzle swizzle = astc::get_swizzle(pixel_type);

		std::uint8_t* image_buffer = (std::uint8_t*)input.data() + input.position();

		astcenc_image encoder_image{};
		encoder_image.dim_x = width;
		encoder_image.dim_y = height;
		encoder_image.dim_z = 1;
		encoder_image.data = (void**)&image_buffer;
		encoder_image.data_type = ASTCENC_TYPE_U8;

		const unsigned int& blocks_x = m_config->block_x;
		const unsigned int& blocks_y = m_config->block_y;

		unsigned int xblocks = (encoder_image.dim_x + blocks_x - 1) / blocks_x;
		unsigned int yblocks = (encoder_image.dim_y + blocks_y - 1) / blocks_y;

		std::size_t data_size = (std::size_t)xblocks * yblocks * 16;
		std::uint8_t* data = Memory::allocate(data_size);

		auto check_status = [&data](astcenc_error& status)
			{
				if (status != ASTCENC_SUCCESS)
				{
					Memory::free(data);
					throw Exception("Failed to ASTC compress data with code %d", (int)status);
				}
			};

		std::vector<astcenc_error> status(m_threads_count, ASTCENC_SUCCESS);
		std::function<void(const astcenc_error&, size_t)> executor = 
			[this, &encoder_image, &swizzle, &data, &data_size, &status](const astcenc_error&, size_t n)
			{
				status[n] = astcenc_compress_image(m_context, &encoder_image, &swizzle, data, data_size, n);
			};

		if (m_threads_count > 1)
		{
			wk::parallel::enumerate(status.begin(), status.end(), executor);
			
		}
		else
		{
			executor(status[0], 0);
		}

		for (auto& thread_status : status)
		{
			check_status(thread_status);
		}

		output.write(data, data_size);
		Memory::free(data);
	}
}