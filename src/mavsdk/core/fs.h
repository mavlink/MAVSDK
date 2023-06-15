#pragma once

#include <string>
#include <optional>
#include <cstdint>

namespace mavsdk {

#if defined(WINDOWS)
const std::string path_separator = "\\";
#else
const std::string path_separator = "/";
#endif

bool fs_exists(const std::string& filename);

uint32_t fs_file_size(const std::string& filename);

std::string fs_filename(const std::string& path);

std::string fs_canonical(const std::string& path);

bool fs_create_directory(const std::string& path);

bool fs_remove(const std::string& path);

bool fs_rename(const std::string& old_name, const std::string& new_name);

std::optional<std::string> create_tmp_directory(const std::string& prefix);

} // namespace mavsdk