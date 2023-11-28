#pragma once

#include <assert.h>

#include "Astc.h"
#include "generic/image/compressed_image.h"
#include "io/buffer_stream.h"

#include "exception/GeneralRuntimeException.h"

#include "generic/ref.h"

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
		/// <summary>
		/// Reads ktx1 file from stream
		/// </summary>
		/// <param name="buffer"></param>
		KhronosTexture(Stream& buffer);

		/// <summary>
		/// Initializes a object with specified format from provided buffer. Buffer is accepted as is and will not be compressed.
		/// </summary>
		/// <param name="format"></param>
		/// <param name="buffer"></param>
		/// <param name="buffer_size"></param>
		KhronosTexture(glInternalFormat format, uint8_t* buffer, size_t buffer_size);

		/// <summary>
		/// Initializes an object from provided Raw Image and compresses it if necessary.
		/// </summary>
		/// <param name="image"></param>
		/// <param name="format"></param>
		KhronosTexture(RawImage& image, glInternalFormat format);

		~KhronosTexture();

	public:
		virtual BasePixelType base_type() const;

		virtual ColorSpace colorspace() const;

		virtual PixelDepth depth() const;

		virtual size_t data_length() const;

		size_t data_length(uint32_t level_index) const;

		virtual uint8_t* data() const;

		const BufferStream* data(uint32_t level_index) const;

		virtual bool is_compressed() const;

		KhronosTextureCompression compression_type();

		virtual size_t decompressed_data_length();

		uint32_t level_count() const;

		/// <summary>
		///
		/// </summary>
		/// <param name="level">Level number</param>
		/// <returns></returns>
		size_t decompressed_data_length(uint32_t level_index);

	public:
		static Image::PixelDepth format_depth(glInternalFormat format);
		static Image::BasePixelType format_type(glFormat format);
		static Image::ColorSpace format_colorspace(glFormat format);
		static bool format_compression(glInternalFormat format);
		static KhronosTextureCompression format_compression_type(glInternalFormat format);

	public:
		virtual void write(Stream& buffer);

		virtual void decompress_data(Stream& output);
		void decompress_data(Stream& output, uint32_t level_index);

		void set_level_data(Stream& data, Image::PixelDepth data_format, uint32_t level_index);

		void reset_level_data(uint32_t level_index);

	private:
		/// <summary>
		/// Reads KTX header
		/// </summary>
		/// <param name="buffer"></param>
		/// <returns> Image levels count </returns>
		uint32_t read_header(Stream& buffer);

		glFormat get_type(glInternalFormat format);

#pragma region
		static void get_astc_blocks(glInternalFormat format, uint32_t& x, uint32_t& y, uint32_t& z);
		void decompress_astc(Stream& input, Stream& output, uint16_t width, uint16_t height);
		void compress_astc(Stream& input, Stream& output);

#pragma endregion ASTC

	private:
		glType m_type;
		glFormat m_format;
		glInternalFormat m_internal_format;

		std::vector<BufferStream*> m_levels;
	};
}