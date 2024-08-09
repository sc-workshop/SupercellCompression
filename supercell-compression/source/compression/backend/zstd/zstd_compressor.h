#pragma once

#include "compression/backend/interface/compressor_interface.h"
#include "compression/backend/zstd/zstd.h"

#include <core/preprocessor/api.h>

#include <thread>

namespace sc
{
	class SUPERCELL_API ZstdCompressor : CompressorInterface
	{
	public:
		struct Props
		{
			/* Set compression parameters according to pre-defined cLevel table.
			   Note that exact compression parameters are dynamically determined,
			   depending on both compression level and srcSize (when known).
			   Default level is ZSTD_CLEVEL_DEFAULT==3.
			   Special: value 0 means default, which is controlled by ZSTD_CLEVEL_DEFAULT.
			   Note 1 : it's possible to pass a negative compression level.
			   Note 2 : setting a level does not automatically set all other compression parameters
				 to default. Setting this will however eventually dynamically impact the compression
				 parameters which have not been manually set. The manually set
				 ones will 'stick'. */
			int compression_level = 3;

			/* Maximum allowed back-reference distance, expressed as power of 2.
			   This will set a memory budget for streaming decompression,
			   with larger values requiring more memory
			   and typically compressing more.
			   Must be clamped between ZSTD_WINDOWLOG_MIN and ZSTD_WINDOWLOG_MAX.
			   Special: value 0 means "use default windowLog".
			   Note: Using a windowLog greater than ZSTD_WINDOWLOG_LIMIT_DEFAULT
					 requires explicitly allowing such size at streaming decompression stage. */
			int window_log = 0;

			/* Size of the initial probe table, as a power of 2.
			   Resulting memory usage is (1 << (hashLog+2)).
			   Must be clamped between ZSTD_HASHLOG_MIN and ZSTD_HASHLOG_MAX.
			   Larger tables improve compression ratio of strategies <= dFast,
			   and improve speed of strategies > dFast.
			   Special: value 0 means "use default hashLog". */
			int hash_log = 0;

			/* Size of the multi-probe search table, as a power of 2.
			   Resulting memory usage is (1 << (chainLog+2)).
			   Must be clamped between ZSTD_CHAINLOG_MIN and ZSTD_CHAINLOG_MAX.
			   Larger tables result in better and slower compression.
			   This parameter is useless for "fast" strategy.
			   It's still useful when using "dfast" strategy,
			   in which case it defines a secondary probe table.
			   Special: value 0 means "use default chainLog". */
			int chain_log = 0;

			/* Number of search attempts, as a power of 2.
			   More attempts result in better and slower compression.
			   This parameter is useless for "fast" and "dFast" strategies.
			   Special: value 0 means "use default searchLog". */
			int search_log = 0;

			/* Minimum size of searched matches.
			   Note that Zstandard can still find matches of smaller size,
			   it just tweaks its search algorithm to look for this size and larger.
			   Larger values increase compression and decompression speed, but decrease ratio.
			   Must be clamped between ZSTD_MINMATCH_MIN and ZSTD_MINMATCH_MAX.
			   Note that currently, for all strategies < btopt, effective minimum is 4.
							   , for all strategies > fast, effective maximum is 6.
			   Special: value 0 means "use default minMatchLength". */
			int min_match = 0;

			/* Impact of this field depends on strategy.
			  For strategies btopt, btultra & btultra2:
				Length of Match considered "good enough" to stop search.
				Larger values make compression stronger, and slower.
			  For strategy fast:
				Distance between match sampling.
				Larger values make compression faster, and weaker.
			  Special: value 0 means "use default targetLength". */
			int target_length = 0;

			/* See ZSTD_strategy enum definition.
			   The higher the value of selected strategy, the more complex it is,
			   resulting in stronger and slower compression.
			   Special: value 0 means "use default strategy". */
			int strategy = 0;

			/* Maximum allowed back-reference distance, expressed as power of 2.
			   This will set a memory budget for streaming decompression,
			   with larger values requiring more memory
			   and typically compressing more.
			   Must be clamped between ZSTD_WINDOWLOG_MIN and ZSTD_WINDOWLOG_MAX.
			   Special: value 0 means "use default windowLog".
			   Note: Using a windowLog greater than ZSTD_WINDOWLOG_LIMIT_DEFAULT
					 requires explicitly allowing such size at streaming decompression stage. */
			int enable_long_distance_matching = 0;

			/* Size of the table for long distance matching, as a power of 2.
			   Larger values increase memory usage and compression ratio,
			   but decrease compression speed.
			   Must be clamped between ZSTD_HASHLOG_MIN and ZSTD_HASHLOG_MAX
			   default: windowlog - 7.
			   Special: value 0 means "automatically determine hashlog". */
			int ldm_hash_log = 0;

			/* Minimum match size for long distance matcher.
			   Larger/too small values usually decrease compression ratio.
			   Must be clamped between ZSTD_LDM_MINMATCH_MIN and ZSTD_LDM_MINMATCH_MAX.
			   Special: value 0 means "use default value" (default: 64). */
			int ldm_min_match = 0;

			/* Log size of each bucket in the LDM hash table for collision resolution.
			   Larger values improve collision resolution but decrease compression speed.
			   The maximum value is ZSTD_LDM_BUCKETSIZELOG_MAX.
			   Special: value 0 means "use default value" (default: 3). */
			int ldm_bucket_size_log = 0;

			/* Frequency of inserting/looking up entries into the LDM hash table.
			   Must be clamped between 0 and (ZSTD_WINDOWLOG_MAX - ZSTD_HASHLOG_MIN).
			   Default is MAX(0, (windowLog - ldmHashLog)), optimizing hash table usage.
			   Larger values improve compression speed.
			   Deviating far from default value will likely result in a compression ratio decrease.
			   Special: value 0 means "automatically determine hashRateLog". */
			int ldm_hash_rate_log = 0;

			/* Content size will be written into frame header _whenever known_ (default:1)
			   Content size must be known at the beginning of compression.
			   This is automatically the case when using ZSTD_compress2(),
			   For streaming scenarios, content size must be provided with ZSTD_CCtx_setPledgedSrcSize() */
			bool content_size_flag = true;

			/* A 32-bits checksum of content is written at end of frame (default:0) */
			bool checksum_flag = false;

			/* When applicable, dictionary's ID is written into frame header (default:1) */
			bool dict_ID_flag = true;

			/* Select how many threads will be spawned to compress in parallel.
			   When nbWorkers >= 1, triggers asynchronous mode when invoking ZSTD_compressStream*() :
			   ZSTD_compressStream*() consumes input and flush output if possible, but immediately gives back control to caller,
			   while compression is performed in parallel, within worker thread(s).
			   (note : a strong exception to this rule is when first invocation of ZSTD_compressStream2() sets ZSTD_e_end :
				in which case, ZSTD_compressStream2() delegates to ZSTD_compress2(), which is always a blocking call).
			   More workers improve speed, but also increase memory usage.
			   Default value is cpu cores count,
			   compression is performed inside Caller's thread, and all invocations are blocking */
			int workers_count = static_cast<int>(std::thread::hardware_concurrency());

			/* Size of a compression job. This value is enforced only when nbWorkers >= 1.
			   Each compression job is completed in parallel, so this value can indirectly impact the nb of active threads.
			   0 means default, which is dynamically determined based on compression parameters.
			   Job size must be a minimum of overlap size, or ZSTDMT_JOBSIZE_MIN (= 512 KB), whichever is largest.
			   The minimum size is automatically and transparently enforced. */
			int job_size = 0;

			/* Control the overlap size, as a fraction of window size.
			   The overlap size is an amount of data reloaded from previous job at the beginning of a new job.
			   It helps preserve compression ratio, while each job is compressed in parallel.
			   This value is enforced only when nbWorkers >= 1.
			   Larger values increase compression ratio, but decrease speed.
			   Possible values range from 0 to 9 :
			   - 0 means "default" : value will be determined by the library, depending on strategy
			   - 1 means "no overlap"
			   - 9 means "full overlap", using a full window size.
			   Each intermediate rank increases/decreases load size by a factor 2 :
			   9: full window;  8: w/2;  7: w/4;  6: w/8;  5:w/16;  4: w/32;  3:w/64;  2:w/128;  1:no overlap;  0:default
			   default value varies between 6 and 9, depending on strategy */
			int overlap_log = 0;
		};

	public:
		ZstdCompressor(Props& props);
		~ZstdCompressor();

		void compress(Stream& input, Stream& output) override;

	private:
		ZSTD_CCtx* m_context = nullptr;

		// -- Stream Buffer --
		const std::size_t Input_Buffer_Size;
		const std::size_t Output_Buffer_Size;

		std::uint8_t* m_input_buffer = nullptr;
		std::uint8_t* m_output_buffer = nullptr;
	};
}
