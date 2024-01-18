#include "SupercellCompression/Zstd.h"

#include <zstd.h>

#include "exception/MemoryAllocationException.h"
#include "SupercellCompression/exception/Zstd.h"
#include "memory/alloc.h"

namespace sc
{
	namespace Compressor
	{
		Zstd::Zstd(Props& props) : Input_Buffer_Size(ZSTD_CStreamInSize()), Output_Buffer_Size(ZSTD_CStreamOutSize())
		{
			m_context = ZSTD_createCCtx();
			if (!m_context)
			{
				throw ZstdCompressInitException();
			}

			ZSTD_CCtx_setParameter(m_context, ZSTD_c_compressionLevel, props.compression_level);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_windowLog, props.window_log);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_hashLog, props.hash_log);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_minMatch, props.min_match);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_targetLength, props.target_length);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_strategy, props.strategy);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_enableLongDistanceMatching, props.enable_long_distance_matching);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_ldmHashLog, props.ldm_hash_log);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_ldmMinMatch, props.ldm_min_match);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_ldmBucketSizeLog, props.ldm_bucket_size_log);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_ldmHashRateLog, props.ldm_hash_rate_log);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_contentSizeFlag, props.content_size_flag);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_checksumFlag, props.checksum_flag);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_dictIDFlag, props.dict_ID_flag);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_nbWorkers, props.workers_count);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_jobSize, props.job_size);
			ZSTD_CCtx_setParameter(m_context, ZSTD_c_overlapLog, props.overlap_log);

			m_input_buffer = memalloc(Input_Buffer_Size);
			m_output_buffer = memalloc(Output_Buffer_Size);
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