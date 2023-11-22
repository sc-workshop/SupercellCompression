#pragma once

#include "Astc.h"
#include "generic/image/compressed_image.h"
#include "io/buffer_stream.h"

#include "exception/GeneralRuntimeException.h"

static const uint8_t KtxFileIdentifier[12] = {
	//'«', 'K', 'T', 'X', ' ', '1', '1', '»', '\r', '\n', '\x1A', '\n'
	0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0A, 0x1A, 0x0A
};

namespace sc
{
	enum class KhronosTextureCompression
	{
		None = 0,
		ASTC
	};

	// TODO: mip maps ?
	// TODO: ETC compression
	class KhronosTexture : public CompressedImage
	{
	public:
		enum class glInternalFormat : uint32_t {
			GL_RGBA8 = 0x8058,
			GL_RGB8 = 0x8051,
			GL_LUMINANCE = 0x1909,
			GL_LUMINANCE_ALPHA = 0x190A,

			// ASTC
			GL_COMPRESSED_RGBA_ASTC_4x4 = 0x93B0,
			GL_COMPRESSED_RGBA_ASTC_5x5 = 0x93B2,
			GL_COMPRESSED_RGBA_ASTC_6x6 = 0x93B4,
			GL_COMPRESSED_RGBA_ASTC_8x8 = 0x93B7,
		};

		enum class glType : uint32_t {
			COMPRESSED = 0,

			//GL_BYTE = 0x1400,
			GL_UNSIGNED_BYTE = 0x1401,
			//GL_SHORT = 0x1402,
			//GL_UNSIGNED_SHORT = 0x1403,
			//GL_HALF_FLOAT = 0x140B,
			//GL_FLOAT = 0x1406,
			//GL_FIXED = 0x140C,
		};

		enum class glFormat : uint32_t {
			UNKNOWN = 0,

			GL_R = 0x1903,
			GL_RG = 0x8227,
			GL_RGB = 0x1907,
			GL_RGBA = 0x1908,
			GL_SRGB = 0x8C40,
			GL_SRGB_ALPHA = 0x8C42
		};

	public:
		KhronosTexture(Stream& buffer)
		{
			size_t data_size = read_header(buffer);

			m_buffer.resize(data_size);
			buffer.read(m_buffer.data(), data_size);
		}

		KhronosTexture(glInternalFormat format, uint8_t* buffer, size_t buffer_size) : m_internalFormat(format)
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

			m_buffer.read(buffer, buffer_size);
		}
	public:
		virtual void write(Stream& buffer)
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
			buffer.write_unsigned_int((uint32_t)m_internalFormat);

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
			buffer.write_unsigned_int(m_mip_map_level_count);

			// bytesOfKeyValueData
			buffer.write_unsigned_int(0);

			// imageSize
			buffer.write_unsigned_int(static_cast<uint32_t>(m_buffer.length()));

			buffer.write(m_buffer.data(), m_buffer.length());
		}

	public:
		virtual BasePixelType base_type() const
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
				break;
			}
		}

		virtual ColorSpace colorspace() const
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
				break;
			}
		}

		virtual PixelDepth depth() const
		{
			switch (m_internalFormat)
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
				break;
			}
		};

		virtual size_t data_length()
		{
			return m_buffer.length();
		}

		virtual uint8_t* data()
		{
			return (uint8_t*)m_buffer.data();
		}

		virtual bool is_compressed() const
		{
			switch (m_internalFormat)
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

		KhronosTextureCompression compression_type()
		{
			switch (m_internalFormat)
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

		virtual size_t decompressed_data_length()
		{
			return Image::calculate_image_length(m_width, m_height, depth());
		}

		virtual void decompress_data(Stream& output)
		{
			switch (compression_type())
			{
			case KhronosTextureCompression::ASTC:
				decompress_astc(output);
				break;

			default:
				output.write(m_buffer.data(), m_buffer.length());
				break;
			}
		}

	private:
		/// <summary>
		/// Reads KTX header
		/// </summary>
		/// <param name="buffer"></param>
		/// <returns> Image data length </returns>
		size_t read_header(Stream& buffer)
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

			m_internalFormat = (glInternalFormat)buffer.read_unsigned_int();
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

			// pixelDepth | must be 0
			uint32_t pixelDepth = buffer.read_unsigned_int();
			if (pixelDepth != 0)
			{
				throw GeneralRuntimeException();
			}

			// numberOfArrayElements | must be 0
			buffer.read_unsigned_int();

			// numberOfFaces | must be 1
			buffer.read_unsigned_int();

			m_mip_map_level_count = buffer.read_unsigned_int();

			uint32_t key_value_data_length = buffer.read_unsigned_int();

			// skip
			buffer.seek(key_value_data_length, Seek::Add);

			return buffer.read_unsigned_int();
		}

		glFormat get_type(glInternalFormat format)
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

		void decompress_astc(Stream& output)
		{
			uint32_t blocks_x = 0;
			uint32_t blocks_y = 0;

			switch (m_internalFormat)
			{
			case sc::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_4x4:
				blocks_x = 4; blocks_y = 4;
				break;
			case sc::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_5x5:
				blocks_x = 5; blocks_y = 5;
				break;
			case sc::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_6x6:
				blocks_x = 6; blocks_y = 6;
				break;
			case sc::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x8:
				blocks_x = 8; blocks_y = 8;
				break;
			default:
				break;
			}

			Decompressor::AstcDecompressProps props;
			props.blocks_x = blocks_x;
			props.blocks_y = blocks_y;
			props.profile = colorspace() == ColorSpace::Linear ? ASTCENC_PRF_LDR : ASTCENC_PRF_LDR_SRGB;

			Decompressor::Astc context(props);
			context.decompress_image(
				m_width, m_height, base_type(),
				MemoryStream((uint8_t*)m_buffer.data(), m_buffer.length()), output
			);
		}

	private:
		glType m_type;
		glFormat m_format;
		glInternalFormat m_internalFormat;

		uint32_t m_mip_map_level_count = 1;

		BufferStream m_buffer;
	};
}