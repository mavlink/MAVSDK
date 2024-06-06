
#include "fs_utils.h"
#include "log.h"
#include <fstream>
#include <random>

#if defined(LINUX) || defined(APPLE)
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef WINDOWS
#include <windows.h>
#include <shlobj.h>
#endif

namespace mavsdk {

#ifdef WINDOWS
static std::optional<std::filesystem::path> get_known_windows_path(REFKNOWNFOLDERID folderId)
{
    std::filesystem::path path;
    PWSTR path_tmp;

    /* Attempt to get user's AppData folder
     *
     * Microsoft Docs:
     * https://learn.microsoft.com/en-us/windows/win32/api/shlobj_core/nf-shlobj_core-shgetknownfolderpath
     * https://learn.microsoft.com/en-us/windows/win32/shell/knownfolderid
     */
    auto get_folder_path_ret = SHGetKnownFolderPath(folderId, 0, nullptr, &path_tmp);

    /* Error check */
    if (get_folder_path_ret != S_OK) {
        CoTaskMemFree(path_tmp);
        return std::nullopt;
    }

    /* Convert the Windows path type to a C++ path */
    path = path_tmp;

    CoTaskMemFree(path_tmp);
    return path;
}
#endif

std::optional<std::filesystem::path> get_cache_directory()
{
    // Windows: %LOCALAPPDATA%
    // Android: /data/data/<package>/cache
    // Linux: ~/.cache
    // macOS: ~/Library/Caches
#if defined(WINDOWS)
    auto path = get_known_windows_path(FOLDERID_LocalAppData);
    if (path) {
        return path->append("mavsdk_cache");
    }
#elif defined(ANDROID)
    // Read /proc/self/cmdline
    std::ifstream cmdline("/proc/self/cmdline");
    std::string line;
    if (std::getline(cmdline, line)) {
        // line might have a trailing \0
        if (line.length() > 0 && *line.end() == 0) {
            line.pop_back();
        }
        return "/data/data/" + line + "/mavsdk_cache";
    }
#elif defined(APPLE) || defined(LINUX)
    const char* homedir;
    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    if (!homedir) {
        return std::nullopt;
    }
#if defined(APPLE)
    return std::filesystem::path(homedir) / "Library" / "Caches" / "mavsdk";
#else // Linux
    return std::filesystem::path(homedir) / ".cache" / "mavsdk";
#endif
#else
#error "Unsupported platform"
#endif

    return std::nullopt;
}

std::optional<std::filesystem::path> create_tmp_directory(const std::string& prefix)
{
    // Inspired by https://stackoverflow.com/a/58454949/8548472
    const auto tmp_dir = std::filesystem::temp_directory_path();

    std::random_device dev;
    std::mt19937 prng(dev());
    std::uniform_int_distribution<uint32_t> rand(0);

    static constexpr unsigned max_tries = 100;

    for (unsigned i = 0; i < max_tries; ++i) {
        std::stringstream ss;
        ss << prefix << '-' << std::hex << rand(prng);
        auto path = tmp_dir / ss.str();

        const auto created = std::filesystem::create_directory(path);
        if (created) {
            return path.string();
        }
    }

    LogErr() << "Could not create a temporary directory, aborting.";
    return std::nullopt;
}
} // namespace mavsdk
