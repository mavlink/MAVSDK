#if defined(WINDOWS)
#include "tronkko_dirent.h"
#include "stackoverflow_unistd.h"
#include <direct.h>
#define mkdir(D, M) _mkdir(D)
#else
#include <dirent.h>
#include <unistd.h>
#endif

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stack>
#include <stdio.h>

#include "fs.h"
#include "filesystem_include.h"
#include "log.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#include <random>

namespace mavsdk {

bool fs_exists(const std::string& filename)
{
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

uint32_t fs_file_size(const std::string& filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return (rc == 0) ? stat_buf.st_size : 0;
}

static void relative_dir_base_split(const std::string& path, std::string& dir, std::string& base)
{
    std::string::size_type slash_pos = path.rfind(path_separator);
    if (slash_pos != std::string::npos) {
        slash_pos++;
        dir = path.substr(0, slash_pos);
        base = path.substr(slash_pos);
    } else {
        dir.clear();
        base = path;
    }
}

std::string fs_filename(const std::string& path)
{
    std::string dir;
    std::string base;
    relative_dir_base_split(path, dir, base);
    return base;
}

std::string fs_canonical(const std::string& path)
{
    std::deque<std::string> st;
    std::string res;

    int len = path.length();

    int index = 0;
    while (index < len) {
        int next_index = path.find(path_separator, index);
        if (next_index == -1) {
            next_index = path.size();
        }

        std::string dir = path.substr(index, next_index - index);

        if (dir.compare("..") == 0 && !st.empty()) {
            st.pop_back();
        } else if (dir.length() != 0 && dir.compare(".") != 0) {
            st.push_back(dir);
        }

        index = next_index + path_separator.size();
    }

    if (path.rfind(path_separator, path_separator.size() - 1) != 0) {
        char buffer[PATH_MAX];
        if (getcwd(buffer, sizeof(buffer)) != nullptr) {
            res.append(buffer);
            if (!st.empty()) {
                res.append(path_separator);
            }
        }
    } else {
        res.append(path_separator);
    }

    while (!st.empty()) {
        std::string temp = st.front();
        if (st.size() != 1) {
            res.append(temp + path_separator);
        } else {
            res.append(temp);
        }
        st.pop_front();
    }

    return res;
}

bool fs_create_directory(const std::string& path)
{
    return (mkdir(path.c_str(), (S_IRWXU | S_IRWXG | S_IRWXO)) == 0);
}

bool fs_remove(const std::string& path)
{
    return (remove(path.c_str()) == 0);
}

bool fs_rename(const std::string& old_name, const std::string& new_name)
{
    return (rename(old_name.c_str(), new_name.c_str()) == 0);
}

// Inspired by https://stackoverflow.com/a/58454949/8548472
std::optional<std::string> create_tmp_directory(const std::string& prefix)
{
    const auto tmp_dir = fs::temp_directory_path();

    std::random_device dev;
    std::mt19937 prng(dev());
    std::uniform_int_distribution<uint32_t> rand(0);

    static constexpr unsigned max_tries = 100;

    for (unsigned i = 0; i < max_tries; ++i) {
        std::stringstream ss;
        ss << prefix << '-' << std::hex << rand(prng);
        auto path = tmp_dir / ss.str();

        const auto created = fs::create_directory(path);
        if (created) {
            return {path.string()};
        }
    }

    LogErr() << "Could not create a temporary directory, aborting.";
    return {};
}

} // namespace mavsdk
