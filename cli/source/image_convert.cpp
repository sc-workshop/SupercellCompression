#include "main.h"
#include "stb/stb.h"

using namespace sc;

#pragma region ASTC
#include "SupercellCompression/Astc.h"
void load_astc(Stream& stream, RawImage** image, CommandLineOptions& options)
{
	uint16_t width;
	uint16_t height;
	Decompressor::AstcDecompressProps props;

	Decompressor::Astc::read_header(stream, width, height, props.blocks_x, props.blocks_y);

	RawImage* image_buffer = new RawImage(width, height, Image::BasePixelType::RGBA, Image::PixelDepth::RGBA8);

	Decompressor::Astc context(props);
	context.decompress_image(
		width, height,
		sc::Image::BasePixelType::RGBA,
		stream, MemoryStream(image_buffer->data(), image_buffer->data_length())
	);

	*image = image_buffer;
}

void write_astc(Stream& stream, RawImage& image, CommandLineOptions& options)
{
	Compressor::AstcCompressProps props;
	props.blocks_x = options.binary.astc.x_blocks;
	props.blocks_y = options.binary.astc.y_blocks;
	// TODO: quality flag

	if (image.depth() != Image::PixelDepth::RGBA8)
	{
		RawImage image_buffer(image.width(), image.height(), Image::BasePixelType::RGBA, Image::PixelDepth::RGBA8);
		image.copy(image_buffer);
		Compressor::Astc::write(image_buffer, props, stream);
	}
	else
	{
		Compressor::Astc::write(image, props, stream);
	}
}
#pragma endregion

bool image_convert(Stream& input_stream, Stream& output_stream, CommandLineOptions& options)
{
	RawImage* image = nullptr;

#pragma region Image Loading
	{
		std::string extension = options.input_path.extension().string();
		make_lowercase(extension);

		if (extension == ".png" ||
			extension == ".jpeg" ||
			extension == ".jpg" ||
			extension == ".psd" ||
			extension == ".bmp" ||
			extension == ".tga")
		{
			stb::load_image(input_stream, &image);
		}
		else if (extension == ".astc")
		{
			load_astc(input_stream, &image, options);
		}
		else
		{
			print("[ERROR] Unknwon input file extension: " << extension);
			return false;
		}
	}
#pragma endregion

	if (image == nullptr)
	{
		print("[ERROR] Failed to load image");
		return false;
	}

#pragma region Image Saving
	{
		std::string extension = options.output_path.extension().string();
		make_lowercase(extension);

		if (extension == ".png" ||
			extension == ".jpeg" ||
			extension == ".jpg" ||
			extension == ".psd" ||
			extension == ".bmp" ||
			extension == ".tga")
		{
			stb::write_image(*image, extension, output_stream);
		}
		else if (extension == ".astc")
		{
			write_astc(output_stream, *image, options);
		}
		else
		{
			print("[ERROR] Unknwon output file extension: " << extension);
			return false;
		}
	}
#pragma endregion

	return true;
}