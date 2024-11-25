#include "compression/backend/astc/astc_decompressor.h"

#include "core/memory/memory.h"
#include "core/exception/exception.h"

#include <astcenc.h>

namespace sc
{
	void ASTCDecompressor::read_astc_header(Stream& input, std::uint16_t& width, std::uint16_t& height, std::uint8_t& blocks_x, std::uint8_t& blocks_y)
	{
		std::uint8_t magic[sizeof(astc::file_identifier)];
		input.read(&magic, sizeof(magic));

		for (std::uint8_t i = 0; sizeof(magic) > i; i++)
		{
			if (magic[i] != astc::file_identifier[i])
			{
				throw Exception("Bad ASTC file magic!");
			}
		}

		// x, y, z blocks
		blocks_x = input.read_unsigned_byte();
		blocks_y = input.read_unsigned_byte();
		input.read_unsigned_byte();

		width = input.read_unsigned_short();
		input.seek(1, Stream::SeekMode::Add);

		height = input.read_unsigned_short();
		input.seek(1, Stream::SeekMode::Add);

		// dim_z
		input.seek(3, Stream::SeekMode::Add);
	}

	ASTCDecompressor::ASTCDecompressor(Props& props)
	{
		astcenc_config config;
		astcenc_error status = astcenc_config_init(
			(astcenc_profile)props.profile,
			props.blocks_x, props.blocks_y, 1,
			ASTCENC_PRE_MEDIUM, ASTCENC_FLG_DECOMPRESS_ONLY,
			&config
		);

		if (status != ASTCENC_SUCCESS)
			throw Exception("Failed to init ASTC with code %d", (int)status);

		status = astcenc_context_alloc(&config, props.threads_count, &m_context);

		if (status != ASTCENC_SUCCESS)
			throw Exception("Failed to allocate ASTC context with code %d", (int)status);
	}

	ASTCDecompressor::~ASTCDecompressor()
	{
		if (m_context)
			astcenc_context_free(m_context);
	}

	void ASTCDecompressor::decompress(std::uint16_t width, std::uint16_t height, Image::BasePixelType pixel_type, Stream& input, Stream& output)
	{
		astcenc_swizzle swizzle = astc::get_swizzle(pixel_type);

		std::size_t data_size = (width * height) * (uint8_t)pixel_type;
		std::uint8_t* data = Memory::allocate(data_size);

		astcenc_image decoder_image;
		decoder_image.dim_x = width;
		decoder_image.dim_y = height;
		decoder_image.dim_z = 1;
		decoder_image.data = reinterpret_cast<void**>(&data);
		decoder_image.data_type = ASTCENC_TYPE_U8;

		std::uint8_t* input_data = (std::uint8_t*)input.data() + input.position();
		std::size_t input_data_length = input.length() - input.position();

		astcenc_error status = astcenc_decompress_image(m_context, input_data, input_data_length, &decoder_image, &swizzle, 0);
		if (status != ASTCENC_SUCCESS)
		{
			Memory::free(data);
			throw Exception("Failed to decompress ASTC data with code %d", (int)status);
		}

		output.write(data, data_size);

		astcenc_decompress_reset(m_context);
		Memory::free(data);
	}
}