#include "fs_helpers.h"
#include "log.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <filesystem>
#include <fstream>
#include <cstring>

using namespace mavsdk;

bool create_temp_file(const fs::path& path, size_t len, uint8_t start)
{
    const auto parent_path = path.parent_path();
    create_directories(parent_path);

    std::ofstream tempfile{};
    tempfile.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (tempfile.fail()) {
        LogWarn() << "Failed to open temp file";
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
    if (ec) {
        LogErr() << "Error removing " << path << ": " << ec.message();
    }

    return fs::create_directories(path);
}

bool are_files_identical(const fs::path& path1, const fs::path& path2)
{
    std::ifstream file1(path1, std::ios::binary);
    std::ifstream file2(path2, std::ios::binary);

    if (!file1) {
        LogWarn() << "Could not open " << path1;
        return false;
    }

    if (!file2) {
        LogWarn() << "Could not open " << path2;
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
