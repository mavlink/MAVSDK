
#pragma once

#include <filesystem>

class InflateLZMA {
public:
    /// Decompresses the specified file to the specified directory
    ///     @param lzma_filename         Fully qualified path to lzma file
    ///     @param decompressed_filename Fully qualified path to for file to decompress to
    static bool inflateLZMAFile(
        const std::filesystem::path& lzma_filename,
        const std::filesystem::path& decompressed_filename);
};
