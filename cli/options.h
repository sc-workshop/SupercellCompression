#pragma once
#include <filesystem>
namespace fs = std::filesystem;

#include "SupercellCompression/KhronosTexture.h"
#include "console.h"
#include <iostream>
#include <thread>

enum class CompressionMethod
{
	LZMA = 0,
	ZSTD,
	LZHAM,
	ASTC
};

enum class FileContainer
{
	None = 0,
	SC,
	ASTC
};

enum class FileFormat
{
	Binary = 0,
	Image
};

enum class Operations
{
	Unknown = 0,

	Compress,
	Decompress,
	Convert
};

#pragma region Images / Textures

struct KhronosOptions
{
	sc::KhronosTexture::glInternalFormat khronos_format = sc::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_4x4;
};

struct ImageOptions
{
	bool save_mip_maps = false;
	bool flip_images = false;

	KhronosOptions khronos;
};
#pragma endregion

#pragma region Binary
//				<--------------- Binary -------------->

struct LzmaOptions
{
	bool use_long_unpacked_length = false;
};

struct SCOptions
{
	bool print_metadata = false;
};

struct ASTCOptions
{
	uint8_t x_blocks = 4;
	uint8_t y_blocks = 4;
};

struct BinaryOptions
{
	CompressionMethod method = CompressionMethod::LZMA;
	FileContainer container = FileContainer::None;

	SCOptions sc;
	LzmaOptions lzma;
	ASTCOptions astc;
};
#pragma endregion

// Helper class to parse options from command line
struct CommandLineOptions
{
	CommandLineOptions(int argc, char* argv[]);

	Operations operation = Operations::Unknown;
	FileFormat file_format = FileFormat::Binary;

	fs::path input_path;
	fs::path output_path;

	unsigned int threads = std::thread::hardware_concurrency();

	BinaryOptions binary;
	ImageOptions image;
};