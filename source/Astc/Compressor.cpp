#include "SupercellCompression/Astc.h"
#include <astcenc.h>

#include "memory/alloc.h"

#include "SupercellCompression/exception/Astc.h"
#include "exception/image/BasicExceptions.h"

using namespace sc::astc;

namespace sc
{
	namespace Compressor
	{
		void Astc::write(Image& image, Props props, Stream& output)
		{
			if (image.is_compressed())
			{
				throw ImageInvalidParamsException();
			}

			output.write(astc::FileIdentifier, sizeof(astc::FileIdentifier));

			// x y z blocks
			output.write_unsigned_byte(props.blocks_x);
			output.write_unsigned_byte(props.blocks_y);
			output.write_unsigned_byte(1);

			uint32_t width = image.width();
			uint32_t height = image.height();
			uint32_t z_dimension = 1;

			// '24-bit' integers for width / height
			output.write(&width, 3);
			output.write(&height, 3);
			output.write(&z_dimension, 3);

			Astc context(props);
			context.compress_image(
				image.width(),
				image.height(),
				image.base_type(),
				MemoryStream(image.data(), image.data_length()),
				output
			);
		}

		Astc::Astc(Props& props)
		{
			m_config = new astcenc_config();
			astcenc_error status;
			status = astcenc_config_init(
				(astcenc_profile)props.profile,
				props.blocks_x, props.blocks_y, 1,
				float(props.quality), 0, m_config
			);

			status = astcenc_context_alloc(m_config, props.threads_count, &m_context);
		};

		Astc::~Astc()
		{
			astcenc_context_free(m_context);
			delete m_config;
		}

		void Astc::compress_image(uint16_t widht, uint16_t height, Image::BasePixelType type, Stream& input, Stream& output)
		{
			astcenc_swizzle swizzle = get_swizzle(type);

			uint8_t* image_buffer = (uint8_t*)input.data() + input.position();

			astcenc_image encoder_image{};
			encoder_image.dim_x = widht;
			encoder_image.dim_y = height;
			encoder_image.dim_z = 1;
			encoder_image.data = (void**)&image_buffer;
			encoder_image.data_type = ASTCENC_TYPE_U8;

			const unsigned int& blocks_x = m_config->block_x;
			const unsigned int& blocks_y = m_config->block_y;

			unsigned int xblocks = (encoder_image.dim_x + blocks_x - 1) / blocks_x;
			unsigned int yblocks = (encoder_image.dim_y + blocks_y - 1) / blocks_y;

			size_t data_size = xblocks * yblocks * 16;
			uint8_t* data = memalloc(data_size);

			astcenc_error status = astcenc_compress_image(m_context, &encoder_image, &swizzle, data, data_size, 0);

			if (status != ASTCENC_SUCCESS)
			{
				free(data);
				throw AstcCompressException();
			}

			output.write(data, data_size);
			free(data);
		};
	}
}