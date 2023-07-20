#pragma once

#include <string>
#include <vector>

#include <filesystem>
namespace fs = std::filesystem;

namespace sc {
	class SwfCache
	{
	public:
		static fs::path GetInfoFilePath(const fs::path& filepath);
		static fs::path GetTempDirectory();
		static fs::path GetTempDirectory(const fs::path& filepath);

		static bool IsFileCached(const fs::path& filepath, std::vector<uint8_t> id, uint32_t fileSize);
		static void ReadCacheInfo(const fs::path& filepath, std::vector<uint8_t>& hash, uint32_t& fileSize);
		static void WriteCacheInfo(const fs::path& filepath, std::vector<uint8_t> hash, uint32_t fileSize);
	};
}