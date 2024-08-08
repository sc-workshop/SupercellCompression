#pragma once

#include <string>
#include <vector>

namespace sc
{
	namespace compression
	{
		namespace flash
		{
			struct MetadataAsset
			{
				std::string name;
				std::vector<char> hash;
			};

			using MetadataAssetArray = std::vector<MetadataAsset>;
		}
	}
}