#pragma once

#include <optional>
#include <filesystem>
#include "mavsdk_export.h"

namespace mavsdk {

/**
 * Get the path to the system's cache directory with a MAVSDK-specific subdirectory.
 * The path does not necessarily exist yet.
 */
MAVSDK_TEST_EXPORT std::optional<std::filesystem::path> get_cache_directory();

/**
 * Create a random subdirectory in the system's tmp directory
 * @param prefix directory prefix
 */
MAVSDK_TEST_EXPORT std::optional<std::filesystem::path>
create_tmp_directory(const std::string& prefix);

MAVSDK_TEST_EXPORT std::string replace_non_ascii_and_whitespace(const std::string& input);

/**
 * Convert a UTF-8 string to a filesystem path without going through the native narrow
 * encoding (which is the ANSI codepage on Windows, not UTF-8).
 */
MAVSDK_TEST_EXPORT std::filesystem::path utf8_path(const std::string& utf8);

/**
 * Convert a filesystem path to a UTF-8 string without going through the native narrow
 * encoding (which is the ANSI codepage on Windows, not UTF-8).
 */
MAVSDK_TEST_EXPORT std::string utf8_string(const std::filesystem::path& path);

} // namespace mavsdk
