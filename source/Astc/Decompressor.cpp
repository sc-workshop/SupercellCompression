#include "SupercellCompression/Astc.h"
#include "memory/alloc.h"

namespace sc
{
	namespace Decompressor
	{
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

			size_t data_size = Image::calculate_image_length(widht, height, type);
			uint8_t* data = memalloc(data_size);

			astcenc_image decoder_image;
			decoder_image.dim_x = widht;
			decoder_image.dim_y = height;
			decoder_image.dim_z = 1;
			decoder_image.data = reinterpret_cast<void**>(&data);
			decoder_image.data_type = ASTCENC_TYPE_U8;

			astcenc_error status = astcenc_decompress_image(m_context, (uint8_t*)input.data(), input.length(), &decoder_image, &swizzle, 0);
			if (!status)
			{
				free(data);
				// TODO: exceptions
				return;
			}

			output.write(data, data_size);
			free(data);
		}
	}
}