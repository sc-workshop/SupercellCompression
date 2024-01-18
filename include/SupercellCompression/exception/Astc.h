#pragma once

#include "astcenc.h"
#include "exception/GeneralRuntimeException.h"

namespace sc
{
	class AstcGeneralException : public GeneralRuntimeException
	{
	public:
		AstcGeneralException(astcenc_error status) : GeneralRuntimeException("AstcGeneralException", 1)
		{
			switch (status)
			{
			case ASTCENC_ERR_OUT_OF_MEM:
				m_message = "Out of memory";
				break;
			case ASTCENC_ERR_BAD_CPU_FLOAT:
				m_message = "Fast math is not supported";
				break;
			case ASTCENC_ERR_BAD_PARAM:
				m_message = "Bad param";
				break;
			case ASTCENC_ERR_BAD_BLOCK_SIZE:
				m_message = "Corrupted block size";
				break;
			case ASTCENC_ERR_BAD_PROFILE:
				m_message = "Corrupted ASTC profile";
				break;
			case ASTCENC_ERR_BAD_QUALITY:
				m_message = "Corrupted ASTC Quality";
				break;
			case ASTCENC_ERR_BAD_SWIZZLE:
				m_message = "Corrupted ASTC Swizzle";
				break;
			case ASTCENC_ERR_BAD_FLAGS:
				m_message = "Bad or corrupted flags";
				break;
			case ASTCENC_ERR_BAD_CONTEXT:
				m_message = "Bad or corrupted ASTC context";
				break;
			case ASTCENC_ERR_NOT_IMPLEMENTED:
				m_message = "Unimplemented functionality";
				break;

			case ASTCENC_SUCCESS:
			default:
				break;
			}
		}
	};
}