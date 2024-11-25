#include "core/io/stream.h"

using namespace wk;

namespace sc
{
	namespace compression
	{
		class ScCommandLineInterface : public CommandLineInterface
		{
		public:
			bool save_metadata = false;
			std::vector<std::string> metadata_paths;

			static void initialize(wk::ArgumentParser& parser)
			{
				parser.add_argument("-scsm", "--sc-save-metadata")
					.flag()
					.help("Save metadata to .metadata.flex while decompressing");

				parser.add_argument("-scmi", "--sc-meta")
					.append()
					.help("Path to .metadata.flex to write while compressing");
			}

			virtual void parse(wk::ArgumentParser& parser)
			{
				save_metadata = parser.get<bool>("--sc-save-metadata");
				metadata_paths = parser.get<std::vector<std::string>>("--sc-meta");
			}

			virtual void decompress(OperationContext& context, Stream& input, Stream& output)
			{
				auto metadata = flash::Decompressor::decompress(input, output);
				if (!metadata.has_value() || !save_metadata) return;

				fs::path output_path = fs::path(context.current_output_file).replace_extension(".metadata.flex");

				OutputFileStream file(output_path);
				std::string data;
				metadata->ToString(true, true, data);
				file.write_string(data);

				std::cout << "Metadata file saved to: " << output_path.make_preferred() << std::endl;
			}

			virtual void compress(OperationContext& context, Stream& input, Stream& output, Method method)
			{
				flash::Signature signature;

				switch (method)
				{
				case Method::LZMA:
					signature = flash::Signature::Lzma;
					break;
				case Method::LZHAM:
					signature = flash::Signature::Lzham;
					break;
				case Method::ZSTD:
					signature = flash::Signature::Zstandard;
					break;
				default:
					throw Exception("Unsupported compression method!");
				}

				sc::compression::flash::Compressor::Context compress_context;
				compress_context.signature = signature;

				// if (metadata_paths.size() > context.current_index)
				// {
				// 	fs::path metadata_path = metadata_paths[context.current_index];
				// 	compress_context.metadata = flexbuffers::Builder();
				// }

				sc::compression::flash::Compressor compressor;
				compressor.compress(input, output, compress_context);
			}
		};
	}
}