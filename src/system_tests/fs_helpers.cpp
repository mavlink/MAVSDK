#include "fs_helpers.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <filesystem>
#include <fstream>
#include <cstring>

bool create_temp_file(const fs::path& path, size_t len, uint8_t start)
{
    const auto parent_path = path.parent_path();
    create_directories(parent_path);

    std::ofstream tempfile{};
    tempfile.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (tempfile.fail()) {
        std::cout << "Failed to open temp file.";
        return false;
    }

    for (size_t i = 0; i < len; ++i) {
        const char c = (i + start) % 256;
        tempfile.write(&c, 1);
    }

    tempfile.close();

    return true;
}

bool reset_directories(const fs::path& path)
{
    std::error_code ec;
    fs::remove_all(path, ec);

    return fs::create_directories(path);
}

bool are_files_identical(const fs::path& path1, const fs::path& path2)
{
    std::ifstream file1(path1, std::ios::binary);
    std::ifstream file2(path2, std::ios::binary);

    if (!file1) {
        std::cout << "Could not open " << path1 << std::endl;
        return false;
    }

    if (!file2) {
        std::cout << "Could not open " << path2 << std::endl;
        return false;
    }

    std::istreambuf_iterator<char> begin1(file1);
    std::istreambuf_iterator<char> begin2(file2);
    std::istreambuf_iterator<char> end;

    return std::equal(begin1, end, begin2, end);
}

bool file_exists(const fs::path& path)
{
    return fs::exists(path);
}
