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

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

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
    std::stack<std::string> st;
    std::string dir;
    std::string res{};

    if (path.rfind(path_separator, 0) != 0) {
        char buffer[PATH_MAX];
        if (getcwd(buffer, sizeof(buffer)) != nullptr) {
            res = std::string(buffer);
        }
    } else {
        res = path_separator;
    }

    int len = path.length();

    for (int i = 0; i < len; i++) {
        dir.clear();
        while (path[i] == '/') {
            i++;
        }
        while (i < len && path[i] != '/') {
            dir.push_back(path[i]);
            i++;
        }
        if (dir.compare("..") == 0 && !st.empty()) {
            st.pop();
        } else if (dir.compare(".") == 0) {
            continue;
        } else if (dir.length() != 0) {
            st.push(dir);
        }
    }

    std::stack<std::string> st1;
    while (!st.empty()) {
        st1.push(st.top());
        st.pop();
    }

    while (!st1.empty()) {
        std::string temp = st1.top();
        if (st1.size() != 1) {
            res.append(temp + "/");
        } else {
            res.append(temp);
        }
        st1.pop();
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
