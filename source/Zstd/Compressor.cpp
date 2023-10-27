#include "SupercellCompression/Zstd.h"

#include "exception/MemoryAllocationException.h"
#include "SupercellCompression/exception/Zstd/CompressException.h"
#include "SupercellCompression/exception/Zstd/CompressInitException.h"

namespace sc
{
	namespace Compressor
	{
		Zstd::Zstd(ZstdProps& props)
		{
			m_context = ZSTD_createCCtx();
			if (!m_context)
			{
				throw ZstdCompressInitException();
			}

			ZSTD_CCtx_setParameter(m_context, ZSTD_c_compressionLevel, props.compressionLevel);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_windowLog, props.windowLog);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_hashLog, props.hashLog);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_minMatch, props.minMatch);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_targetLength, props.targetLength);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_strategy, props.strategy);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_enableLongDistanceMatching, props.enableLongDistanceMatching);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_ldmHashLog, props.ldmHashLog);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_ldmMinMatch, props.ldmMinMatch);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_ldmBucketSizeLog, props.ldmBucketSizeLog);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_ldmHashRateLog, props.ldmHashRateLog);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_contentSizeFlag, props.contentSizeFlag);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_checksumFlag, props.checksumFlag);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_dictIDFlag, props.dictIDFlag);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_nbWorkers, props.nbWorkers);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_jobSize, props.jobSize);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_overlapLog, props.overlapLog);

			m_input_buffer = (uint8_t*)malloc(Input_Buffer_Size);
			if (!m_input_buffer)
			{
				throw MemoryAllocationException(Input_Buffer_Size);
			}

			m_output_buffer = (uint8_t*)malloc(Output_Buffer_Size);
			if (!m_output_buffer)
			{
				if (m_input_buffer) { free(m_input_buffer); }
				throw MemoryAllocationException(Output_Buffer_Size);
			}
		}

		Zstd::~Zstd()
		{
			if (m_input_buffer)
			{
				free(m_input_buffer);
			}

			if (m_output_buffer)
			{
				free(m_output_buffer);
			}

			ZSTD_freeCCtx(m_context);
		}

		void Zstd::compress_stream(Stream& input, Stream& output)
		{
			ZSTD_CCtx_setPledgedSrcSize(m_context, input.length() - input.position());

			size_t remain_bytes = Input_Buffer_Size;
			while (true) {
				size_t byteCount = input.read(m_input_buffer, remain_bytes);

				const int last_chunk = (byteCount < remain_bytes);
				const ZSTD_EndDirective mode = last_chunk ? ZSTD_e_end : ZSTD_e_continue;
				ZSTD_inBuffer input_buffer = { m_input_buffer, byteCount, 0 };
				int finished = 0;
				while (!finished) {
					ZSTD_outBuffer output_buffer = { m_output_buffer, Output_Buffer_Size, 0 };
					size_t const remaining = ZSTD_compressStream2(m_context, &output_buffer, &input_buffer, mode);
					output.write(m_output_buffer, output_buffer.pos);
					finished = last_chunk ? (remaining == 0) : (input_buffer.pos == input_buffer.size);
				};
				if (input_buffer.pos != input_buffer.size)
				{
					throw ZstdCompressException();
				}

				if (last_chunk) {
					break;
				}
			}
		}
	}
}