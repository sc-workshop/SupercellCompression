#pragma once

#pragma region Forward Declaration

struct ZSTD_CCtx_s;
typedef struct ZSTD_CCtx_s ZSTD_CCtx;

struct ZSTD_DCtx_s;
typedef struct ZSTD_DCtx_s ZSTD_DCtx;
typedef ZSTD_DCtx ZSTD_DStream;

#pragma endregion

#include "Zstd/Compressor.h"
#include "Zstd/Decompressor.h"