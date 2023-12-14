set(CompressionCLI_Headers
    "cli/console.h"
    "cli/main.h"
    "cli/options.h"
)

set(CompressionCLI_Source
    "cli/compress.cpp"
    "cli/console.cpp"
    "cli/decompress.cpp"
    "cli/image_convert.cpp"
    "cli/main.cpp"
    "cli/options.cpp"
)

add_executable("SupercellCompressionCLI"
    ${CompressionCLI_Headers} ${CompressionCLI_Source}
)
sc_core_base_setup("SupercellCompressionCLI")
set_target_properties("SupercellCompressionCLI" PROPERTIES
    FOLDER Supercell/CLI
)

target_link_libraries("SupercellCompressionCLI" PUBLIC
    SupercellCompression
)