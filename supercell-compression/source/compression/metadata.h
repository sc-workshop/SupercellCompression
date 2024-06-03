#pragma once

#include <string>
#include <vector>

namespace sc
{
	struct MetadataAsset
	{
		std::string name;
		std::vector<char> hash;
	};

	using MetadataAssetArray = std::vector<MetadataAsset>;
}