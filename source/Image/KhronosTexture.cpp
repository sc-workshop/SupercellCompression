#include "SupercellCompression/KhronosTexture.h"

namespace sc
{
#pragma region
	KhronosTexture::KhronosTexture(Stream& buffer)
	{
		uint32_t levels_count = read_header(buffer);

		m_levels.resize(levels_count);
		for (uint32_t level_index = 0; levels_count > level_index; level_index++)
		{
			uint32_t level_length = buffer.read_unsigned_int();

			m_levels[level_index] = new MemoryStream(level_length);
			buffer.read(m_levels[level_index]->data(), level_length);
		}
	}

	KhronosTexture::KhronosTexture(glInternalFormat format, uint8_t* buffer, size_t buffer_size) : m_internal_format(format)
	{
		m_format = get_type(format);

		if (!is_compressed())
		{
			m_type = glType::GL_UNSIGNED_BYTE;
		}
		else
		{
			m_type = glType::COMPRESSED;
		}

		MemoryStream* stream = new MemoryStream(buffer_size);
		stream->write(buffer, buffer_size);
		stream->seek(0);

		m_levels.push_back(stream);
	}

	KhronosTexture::KhronosTexture(RawImage& image, glInternalFormat format) : m_internal_format(format)
	{
		m_format = get_type(format);
		m_width = image.width();
		m_height = image.height();

		uint8_t* buffer = nullptr;

		// Converting Depth
		Image::PixelDepth source_depth = image.depth();
		Image::PixelDepth destination_depth = depth();

		if (source_depth != destination_depth)
		{
			buffer = memalloc(Image::calculate_image_length(m_width, m_height, destination_depth));
			Image::remap(
				image.data(), buffer,
				m_width, m_height,
				source_depth, destination_depth
			);
		}

		if (is_compressed())
		{
			m_type = glType::COMPRESSED;
			// TODO: Image compression yeah
		}
		else
		{
			m_type = glType::GL_UNSIGNED_BYTE;

			size_t image_length = Image::calculate_image_length(m_width, m_height, depth());

			MemoryStream* stream = new MemoryStream(image_length);
			stream->write(buffer ? buffer : image.data(), image_length);
			stream->seek(0);

			m_levels.push_back(stream);
		}

		if (buffer)
		{
			free(buffer);
		}
	}
#pragma endregion Constructors

#pragma region
	void KhronosTexture::write(Stream& buffer)
	{
		bool is_compressed = m_type == glType::COMPRESSED;

		buffer.write(&KtxFileIdentifier, sizeof(KtxFileIdentifier));

		// endianess
		buffer.write_unsigned_int(0x04030201);

		// glType
		buffer.write_unsigned_int((uint32_t)m_type);

		// glTypeSize
		{
			uint32_t glTypeSize = 0;
			if (is_compressed)
			{
				glTypeSize = 1;
			}

			buffer.write_unsigned_int(glTypeSize);
		}

		// glFormat
		buffer.write_unsigned_int(is_compressed ? 0 : (uint32_t)m_format);

		// glInternalFormat
		buffer.write_unsigned_int((uint32_t)m_internal_format);

		// glBaseInternalType
		buffer.write_unsigned_int((uint32_t)m_format);

		// Width / height
		buffer.write_unsigned_int(m_width);
		buffer.write_unsigned_int(m_height);

		// pixelDepth
		buffer.write_unsigned_int(0);

		// numberOfArrayElements
		buffer.write_unsigned_int(0);

		// numberOfFaces
		buffer.write_unsigned_int(1);

		// numberOfMipmapLevels
		buffer.write_unsigned_int(static_cast<uint32_t>(m_levels.size()));

		// bytesOfKeyValueData
		buffer.write_unsigned_int(0);

		for (MemoryStream* level : m_levels)
		{
			uint32_t image_length = static_cast<uint32_t>(level->length());
			buffer.write_unsigned_int(image_length);
			buffer.write(level->data(), image_length);
		}
	}

	void KhronosTexture::decompress_data(Stream& output, uint32_t level_index)
	{
		if (level_index >= m_levels.size()) level_index = static_cast<uint32_t>(m_levels.size()) - 1;

		switch (compression_type())
		{
		case KhronosTextureCompression::ASTC:
			decompress_astc(output, level_index);
			break;

		default:
			output.write(m_levels[level_index]->data(), m_levels[level_index]->length());
			break;
		}
	}
#pragma endregion Functions

#pragma region
	Image::BasePixelType KhronosTexture::base_type() const
	{
		switch (m_format)
		{
		case sc::KhronosTexture::glFormat::GL_R:
			return BasePixelType::L;

		case sc::KhronosTexture::glFormat::GL_RG:
			return BasePixelType::LA;

		case sc::KhronosTexture::glFormat::GL_SRGB:
		case sc::KhronosTexture::glFormat::GL_RGB:
			return BasePixelType::RGB;

		case sc::KhronosTexture::glFormat::GL_SRGB_ALPHA:
		case sc::KhronosTexture::glFormat::GL_RGBA:
			return BasePixelType::RGBA;

		default:
			assert(0 && "Unknown glFormat");
			break;
		}
	}

	Image::ColorSpace KhronosTexture::colorspace() const
	{
		switch (m_format)
		{
		case sc::KhronosTexture::glFormat::GL_RGBA:
		case sc::KhronosTexture::glFormat::GL_RGB:
		case sc::KhronosTexture::glFormat::GL_RG:
		case sc::KhronosTexture::glFormat::GL_R:
			return ColorSpace::Linear;

		case sc::KhronosTexture::glFormat::GL_SRGB:
		case sc::KhronosTexture::glFormat::GL_SRGB_ALPHA:
			return ColorSpace::sRGB;

		default:
			assert(0 && "Unknown glFormat");
			break;
		}
	}

	Image::PixelDepth KhronosTexture::depth() const
	{
		switch (m_internal_format)
		{
		case sc::KhronosTexture::glInternalFormat::GL_RGBA8:
			return PixelDepth::RGBA8;
		case sc::KhronosTexture::glInternalFormat::GL_RGB8:
			return PixelDepth::RGB8;
		case sc::KhronosTexture::glInternalFormat::GL_LUMINANCE:
			return PixelDepth::LUMINANCE8;
		case sc::KhronosTexture::glInternalFormat::GL_LUMINANCE_ALPHA:
			return PixelDepth::LUMINANCE8_ALPHA8;

		case sc::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_4x4:
		case sc::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_5x5:
		case sc::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_6x6:
		case sc::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x8:
			return PixelDepth::RGBA8;

		default:
			assert(0 && "Unknown glInternalFormat");
			break;
		}
	};

	size_t KhronosTexture::data_length() const
	{
		return data_length(0);
	}

	size_t KhronosTexture::data_length(uint32_t level_index) const
	{
		if (level_index >= m_levels.size()) level_index = static_cast<uint32_t>(m_levels.size()) - 1;

		return m_levels[level_index]->length();
	}

	uint8_t* KhronosTexture::data() const
	{
		return data(0);
	}

	uint8_t* KhronosTexture::data(uint32_t level_index) const
	{
		if (level_index >= m_levels.size()) level_index = static_cast<uint32_t>(m_levels.size()) - 1;

		return (uint8_t*)m_levels[level_index]->data();
	}

	bool KhronosTexture::is_compressed() const
	{
		switch (m_internal_format)
		{
		case sc::KhronosTexture::glInternalFormat::GL_RGBA8:
		case sc::KhronosTexture::glInternalFormat::GL_RGB8:
		case sc::KhronosTexture::glInternalFormat::GL_LUMINANCE:
		case sc::KhronosTexture::glInternalFormat::GL_LUMINANCE_ALPHA:
			return false;

		case sc::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_4x4:
		case sc::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_5x5:
		case sc::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_6x6:
		case sc::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x8:
			return true;

		default:
			break;
		}
	}

	KhronosTextureCompression KhronosTexture::compression_type()
	{
		switch (m_internal_format)
		{
		case sc::KhronosTexture::glInternalFormat::GL_RGBA8:
		case sc::KhronosTexture::glInternalFormat::GL_RGB8:
		case sc::KhronosTexture::glInternalFormat::GL_LUMINANCE:
		case sc::KhronosTexture::glInternalFormat::GL_LUMINANCE_ALPHA:
			return KhronosTextureCompression::None;

		case sc::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_4x4:
		case sc::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_5x5:
		case sc::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_6x6:
		case sc::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x8:
			return KhronosTextureCompression::ASTC;
		default:
			break;
		}
	}

	size_t KhronosTexture::decompressed_data_length()
	{
		return decompressed_data_length(0);
	}

	size_t KhronosTexture::decompressed_data_length(uint32_t level)
	{
		return Image::calculate_image_length(m_width / level, m_height / level, depth());
	}

	void KhronosTexture::decompress_data(Stream& output)
	{
		return decompress_data(output, 0);
	}
#pragma endregion Getters/Setters

#pragma region
	uint32_t KhronosTexture::read_header(Stream& buffer)
	{
		for (uint8_t i = 0; sizeof(KtxFileIdentifier) > i; i++)
		{
			if (buffer.read_byte() != KtxFileIdentifier[i])
			{
				// TODO: exception
			}
		}

		// endianess
		buffer.read_unsigned_int();

		m_type = (glType)buffer.read_unsigned_int();

		// glTypeSize
		buffer.read_unsigned_int();

		glFormat format = (glFormat)buffer.read_unsigned_int();

		m_internal_format = (glInternalFormat)buffer.read_unsigned_int();
		glFormat internalBasetype = (glFormat)buffer.read_unsigned_int();

		if (format == glFormat::UNKNOWN)
		{
			m_format = internalBasetype;
		}
		else
		{
			m_format = format;
		}

		m_width = static_cast<uint16_t>(buffer.read_unsigned_int());
		m_height = static_cast<uint16_t>(buffer.read_unsigned_int());

		// pixelDepth | must be 0											//
		assert(buffer.read_unsigned_int() == 0 && "Pixel Depth != 0");		//
																			//
		// numberOfArrayElements | must be 0								// Some hardcoded/unsuported values
		assert(buffer.read_unsigned_int() == 0 && "Array elements != 0");	//
																			//
		// numberOfFaces | must be 1										//
		assert(buffer.read_unsigned_int() == 1 && "Faces number != 1");		//

		uint32_t levels_count = buffer.read_unsigned_int();

		uint32_t key_value_data_length = buffer.read_unsigned_int();

		// skip
		buffer.seek(key_value_data_length, Seek::Add);

		return levels_count;
	}

	KhronosTexture::glFormat KhronosTexture::get_type(glInternalFormat format)
	{
		switch (format)
		{
		case sc::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x8:
		case sc::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_6x6:
		case sc::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_5x5:
		case sc::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_4x4:
		case sc::KhronosTexture::glInternalFormat::GL_RGBA8:
			return glFormat::GL_RGBA;

		case sc::KhronosTexture::glInternalFormat::GL_RGB8:
			return glFormat::GL_RGB;
		case sc::KhronosTexture::glInternalFormat::GL_LUMINANCE:
			return glFormat::GL_RG;
		case sc::KhronosTexture::glInternalFormat::GL_LUMINANCE_ALPHA:
			return glFormat::GL_R;

		default:
			break;
		}
	};
#pragma endregion Private Functions

#pragma region
	void KhronosTexture::get_astc_blocks(glInternalFormat format, uint32_t& x, uint32_t& y, uint32_t& z)
	{
		switch (format)
		{
		case glInternalFormat::GL_COMPRESSED_RGBA_ASTC_4x4:
			x = 4; y = 4; z = 0;
			break;
		case glInternalFormat::GL_COMPRESSED_RGBA_ASTC_5x5:
			x = 5; y = 5; z = 0;
			break;
		case glInternalFormat::GL_COMPRESSED_RGBA_ASTC_6x6:
			x = 6; y = 6; z = 0;
			break;
		case glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x8:
			x = 8; y = 8; z = 0;
			break;
		default:
			x = 0; y = 0; z = 0;
			break;
		}
	}

	void KhronosTexture::decompress_astc(Stream& output, uint32_t level_index)
	{
		uint32_t blocks_x;
		uint32_t blocks_y;
		uint32_t blocks_z;
		get_astc_blocks(m_internal_format, blocks_x, blocks_y, blocks_z);

		Decompressor::AstcDecompressProps props;
		props.blocks_x = blocks_x;
		props.blocks_y = blocks_y;
		props.profile = colorspace() == ColorSpace::Linear ? ASTCENC_PRF_LDR : ASTCENC_PRF_LDR_SRGB;

		MemoryStream* buffer = m_levels[level_index];

		Decompressor::Astc context(props);
		context.decompress_image(
			m_width, m_height, base_type(),
			*buffer, output
		);
	}
#pragma endregion ASTC Compression
}