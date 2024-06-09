#pragma once

#include <cstdint>

namespace sc
{
	const std::uint16_t SC_MAGIC = 0x4353;
	const std::uint32_t SC_LZHAM_MAGIC = 0x5A4C4353;
	const std::uint32_t SC_SHA64_MAGIC = 0x3A676953;

	enum class Signature
	{
		None,

		Lzma,
		Lzham,
		Zstandard,

		Sha64
	};
}
