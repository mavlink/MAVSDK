#pragma once

#include <optional>
#include <filesystem>

namespace mavsdk {

/**
 * Get the path to the system's cache directory with a MAVSDK-specific subdirectory.
 * The path does not necessarily exist yet.
 */
std::optional<std::filesystem::path> get_cache_directory();

/**
 * Create a random subdirectory in the system's tmp directory
 * @param prefix directory prefix
 */
std::optional<std::filesystem::path> create_tmp_directory(const std::string& prefix);

} // namespace mavsdk
