#pragma once

#include <cstddef>
#include <filesystem>

namespace fs = std::filesystem;

bool create_temp_file(const fs::path& path, size_t len, uint8_t start = 0);

bool reset_directories(const fs::path& path);

bool are_files_identical(const fs::path& path1, const fs::path& path2);

bool file_exists(const fs::path& path);
