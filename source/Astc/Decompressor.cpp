#include "SupercellCompression/Astc.h"
#include "memory/alloc.h"

#include "SupercellCompression/exception/Astc.h"
#include "exception/io/BinariesExceptions.h"

namespace sc
{
	namespace Decompressor
	{
		void Astc::read_header(Stream& buffer, uint16_t& width, uint16_t& height, uint8_t& blocks_x, uint8_t& blocks_y)
		{
			uint8_t magic[sizeof(AstcFileIdentifier)];
			buffer.read(&magic, sizeof(magic));

			for (uint8_t i = 0; sizeof(magic) > i; i++)
			{
				if (magic[i] != AstcFileIdentifier[i])
				{
					throw BadMagicException((uint8_t*)&AstcFileIdentifier, (uint8_t*)&magic, sizeof(magic));
				}
			}

			// x, y, z blocks
			blocks_x = buffer.read_unsigned_byte();
			blocks_y = buffer.read_unsigned_byte();
			buffer.read_unsigned_byte();

			width = buffer.read_unsigned_short();
			buffer.seek(1, Seek::Add);

			height = buffer.read_unsigned_short();
			buffer.seek(1, Seek::Add);

			// dim_z
			buffer.seek(3, Seek::Add);
		};

		Astc::Astc(AstcDecompressProps& props)
		{
			astcenc_error status;

			astcenc_config config;
			status = astcenc_config_init(
				props.profile,
				props.blocks_x, props.blocks_y, 1,
				ASTCENC_PRE_MEDIUM, ASTCENC_FLG_DECOMPRESS_ONLY,
				&config
			);

			if (status != ASTCENC_SUCCESS)
			{
				// TODO: exception
			}

			status = astcenc_context_alloc(&config, props.threads_count, &m_context);

			if (status != ASTCENC_SUCCESS)
			{
				// TODO: exception
			}
		}

		Astc::~Astc()
		{
			if (m_context)
			{
				astcenc_context_free(m_context);
			}
		}

		void Astc::decompress_image(uint16_t widht, uint16_t height, Image::BasePixelType type, Stream& input, Stream& output)
		{
			astcenc_swizzle swizzle = get_astc_swizzle(type);

			size_t data_size = (widht * height) * (uint8_t)type;
			uint8_t* data = memalloc(data_size);

			astcenc_image decoder_image;
			decoder_image.dim_x = widht;
			decoder_image.dim_y = height;
			decoder_image.dim_z = 1;
			decoder_image.data = reinterpret_cast<void**>(&data);
			decoder_image.data_type = ASTCENC_TYPE_U8;

			uint8_t* input_data = (uint8_t*)input.data() + input.position();
			size_t input_data_length = input.length() - input.position();

			astcenc_error status = astcenc_decompress_image(m_context, input_data, input_data_length, &decoder_image, &swizzle, 0);
			if (status != ASTCENC_SUCCESS)
			{
				free(data);
				// TODO: exceptions
				return;
			}

			output.write(data, data_size);

			astcenc_decompress_reset(m_context);
			free(data);
		}
	}
}