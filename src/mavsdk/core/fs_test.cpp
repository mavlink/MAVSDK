#include <gtest/gtest.h>

#include "fs.h"
#include "unused.h"

#if !defined(WINDOWS)
#define PATH_MAX 4096

#include <unistd.h>

TEST(Filesystem, AbsolutePathUnchanged)
{
    const std::string path = "/sbin/init";
    ASSERT_EQ(path, mavsdk::fs_canonical(path));
}

TEST(Filesystem, RemoveFinalSlash)
{
    const std::string path = "/usr/local/include/mavsdk/";
    const std::string canonical_path = "/usr/local/include/mavsdk";
    ASSERT_EQ(canonical_path, mavsdk::fs_canonical(path));
}

TEST(Filesystem, RemoveDuplicateSlashes)
{
    const std::string path = "//////opt/////ros////foxy";
    const std::string canonical_path = "/opt/ros/foxy";
    ASSERT_EQ(canonical_path, mavsdk::fs_canonical(path));
}

TEST(Filesystem, RemoveInternalDots)
{
    const std::string path = "/sys/./class/./input";
    const std::string canonical_path = "/sys/class/input";
    ASSERT_EQ(canonical_path, mavsdk::fs_canonical(path));
}

TEST(Filesystem, ResolveDoubleDots)
{
    const std::string path = "/bin/../dev/../etc/crontab/../shadow";
    const std::string canonical_path = "/etc/shadow";
    ASSERT_EQ(canonical_path, mavsdk::fs_canonical(path));
}

TEST(Filesystem, RelativePathEmpty)
{
    char cwd[PATH_MAX];
    UNUSED(getcwd(cwd, PATH_MAX));
    const std::string path = "";
    const std::string canonical_path = std::string(cwd);
    ASSERT_EQ(canonical_path, mavsdk::fs_canonical(path));
}

TEST(Filesystem, RelativePathDot)
{
    char cwd[PATH_MAX];
    UNUSED(getcwd(cwd, PATH_MAX));
    const std::string path = ".";
    const std::string canonical_path = std::string(cwd);
    ASSERT_EQ(canonical_path, mavsdk::fs_canonical(path));
}

TEST(Filesystem, RelativePathBare)
{
    char cwd[PATH_MAX];
    UNUSED(getcwd(cwd, PATH_MAX));
    const std::string path = "src/mavsdk/core/fs_test.cpp";
    const std::string canonical_path = std::string(cwd) + mavsdk::path_separator + path;
    ASSERT_EQ(canonical_path, mavsdk::fs_canonical(path));
}

TEST(Filesystem, RelativePathDotSlash)
{
    char cwd[PATH_MAX];
    UNUSED(getcwd(cwd, PATH_MAX));
    const std::string bare_path = "src/mavsdk/plugins/mavlink_passthrough";
    const std::string dotslash_path = "./" + bare_path;
    const std::string canonical_path = std::string(cwd) + mavsdk::path_separator + bare_path;
    ASSERT_EQ(canonical_path, mavsdk::fs_canonical(dotslash_path));
}

#endif
