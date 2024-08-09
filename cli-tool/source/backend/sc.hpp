namespace sc
{
	namespace compression
	{
		class ScCommandLineInterface : public CommandLineInterface
		{
		public:
			bool save_metadata = false;
			std::vector<std::string> metadata_paths;

			static void initialize(sc::ArgumentParser& parser)
			{
				parser.add_argument("-scsm", "--sc-save-metadata")
					.flag()
					.help("Save metadata to .metadata.flex while decompressing");

				parser.add_argument("-scmi", "--sc-meta")
					.append()
					.help("Path to .metadata.flex to write while compressing");
			}

			virtual void parse(sc::ArgumentParser& parser)
			{
				save_metadata = parser.get<bool>("--sc-save-metadata");
				metadata_paths = parser.get<std::vector<std::string>>("--sc-meta");
			}

			virtual void decompress(OperationContext& context, sc::Stream& input, sc::Stream& output)
			{
				if (save_metadata)
				{
					sc::MemoryStream* metadata = nullptr;
					flash::Decompressor::decompress(input, output, &metadata);

					if (metadata != nullptr)
					{
						fs::path output_path = fs::path(context.current_output_file).replace_extension(".metadata.flex");

						sc::OutputFileStream file(output_path);
						file.write(metadata->data(), metadata->length());

						std::cout << "Metadata file saved to: " << output_path.make_preferred() << std::endl;
						delete metadata;
					}
				}
				else
				{
					flash::Decompressor::decompress(input, output);
				}
			}

			virtual void compress(OperationContext& context, sc::Stream& input, sc::Stream& output, Method method)
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
					throw sc::Exception("Unsupported compression method!");
				}

				sc::compression::flash::Compressor::Context compress_context;
				compress_context.signature = signature;

				if (metadata_paths.size() > context.current_index)
				{
					fs::path metadata_path = metadata_paths[context.current_index];
					compress_context.metadata = sc::CreateRef<sc::InputFileStream>(metadata_path);
				}

				sc::compression::flash::Compressor compressor;
				compressor.compress(input, output, compress_context);
			}
		};
	}
}