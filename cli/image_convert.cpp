#include "main.h"
#include "stb/stb.h"

#include <vector>
using std::vector;

using namespace sc;

#pragma region ASTC
#include "SupercellCompression/Astc.h"

void load_khronos(Stream& stream, vector<RawImage*>& output, CommandLineOptions&)
{
	KhronosTexture texture(stream);

	uint32_t mips_count = texture.level_count();
	output.reserve(mips_count);
	for (uint32_t level_index = 0; mips_count > level_index; level_index++)
	{
		RawImage* image = new RawImage(
			texture.width() / (uint16_t)(pow(2, level_index)),
			texture.height() / (uint16_t)(pow(2, level_index)),
			texture.base_type(),
			texture.depth()
		);

		MemoryStream image_data(image->data(), image->data_length());

		texture.decompress_data(
			image_data, level_index
		);

		output.push_back(image);
	}
}

void load_astc(Stream& stream, RawImage** image, CommandLineOptions&)
{
	uint16_t width;
	uint16_t height;
	Decompressor::Astc::Props props;

	Decompressor::Astc::read_header(stream, width, height, props.blocks_x, props.blocks_y);

	RawImage* image_buffer = new RawImage(width, height, Image::BasePixelType::RGBA, Image::PixelDepth::RGBA8);
	MemoryStream image_data(image_buffer->data(), image_buffer->data_length());

	Decompressor::Astc context(props);
	context.decompress_image(
		width, height,
		sc::Image::BasePixelType::RGBA,
		stream, image_data
	);

	*image = image_buffer;
}

void write_astc(Stream& stream, RawImage& image, CommandLineOptions& options)
{
	Compressor::Astc::Props props;
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

bool image_convert(Stream& input_stream, CommandLineOptions& options)
{
	vector<RawImage*> images;

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
			RawImage* image = nullptr;
			stb::load_image(input_stream, &image);

			images.push_back(image);
		}
		else if (extension == ".astc")
		{
			RawImage* image = nullptr;
			load_astc(input_stream, &image, options);

			images.push_back(image);
		}
		else if (extension == ".ktx")
		{
			load_khronos(input_stream, images, options);
		}
		else
		{
			print("[ERROR] Unknwon input file extension: " << extension);
			return false;
		}
	}
#pragma endregion

#pragma region Image Saving
	{
		std::string extension = options.output_path.extension().string();
		make_lowercase(extension);

		for (uint32_t i = 0; images.size() > i; i++)
		{
			if (images[i] == nullptr)
			{
				print("[ERROR] Failed to load image " << std::to_string(i));
				continue;
			}

			RawImage& image = *images.at(i);

			fs::path output_path = options.output_path;
			if (!options.image.save_mip_maps && i >= 1)
			{
				break;
			}

			if (options.image.save_mip_maps)
			{
				output_path = fs::path(
					output_path.parent_path() /
					output_path.stem()
					.concat("_")
					.concat(std::to_string(i))
					.concat(extension)
				);
			}

			OutputFileStream output_stream(output_path);

			if (extension == ".png" ||
				extension == ".jpeg" ||
				extension == ".jpg" ||
				extension == ".psd" ||
				extension == ".bmp" ||
				extension == ".tga")
			{
				stb::write_image(image, extension, output_stream);
			}
			else if (extension == ".astc")
			{
				write_astc(output_stream, image, options);
			}
			else
			{
				print("[ERROR] Unknwon output file extension: " << extension);
				break;
			}

			delete images.at(i);
			images[i] = nullptr;
		}

		for (RawImage* image : images)
		{
			if (image != nullptr) delete image;
		}
	}
#pragma endregion

	return true;
}